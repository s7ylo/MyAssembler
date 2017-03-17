/*
 * Assembler.c
 */

#include "Assembler.h"
#include "logs.h"

static program_object_t initialize_program_object(void)
{
	program_object_t prog_obj = calloc(1, sizeof(program_object));

	if (!prog_obj)
		return NULL;

	prog_obj->sym_tbl = NULL;
	prog_obj->ic = (word_t)calloc(1, sizeof(word));
	prog_obj->dc = (word_t)calloc(1, sizeof(word));

	return prog_obj;
}

void handle_symbol(
		program_object_t prog_obj,
		const char *name,
		bool external,
		bool inst,
		bool data,
		word_t data_size,
		word_t address)
{
	symbol_t sym;

	if (strlen(name) > MAX_SYMBOL_NAME_LENGTH)
	{
		print_log(
			"%s Symbol name: %s exceeds the allowed max length\n",
			ERROR,
			name);

		return;
	}

	if (is_symbol_exist_in_table(prog_obj->sym_tbl, name))
	{
		print_log(
			"%s Symbol name: %s is already exist\n",
			ERROR,
			name);

		return;
	}

	sym = sym_alloc();

	strncpy(
		sym->name,
		name,
		strlen(name));

	sym->is_external = external;
	sym->is_instruction = inst;
	sym->is_data = data;

	if (data_size)
		sym->data_size.data = data_size->data;

	if (address)
		sym->address.data = address->data;
	else
		sym->address.data = 0;

	insert_symbol_to_table(&prog_obj->sym_tbl, sym);
}

/* at the first transition we pass the code and addressing only the following
 *  - keeping a counter of the data and code sections, the code count starts from 100 and the data from 0
 *  - as each instruction passes, we increment the code section counter by the size of the instruction
 *  - as each data pass, we increment the data section counter by the size of data
 *  - symbols, we save every symbol we found (external or not) and their address (ignore external address)
 *  - we ignore the '.entry' guidance statement at the first transition
 */
static
void
assembler_first_transition_single_line(
		const char *source_line,
		program_object_t prog_obj,
		u_short line_number)
{
	char *source_line_cpy = strdup(source_line);
	char *source_line_e;
	char *source_line_token;
	char *directive;
	char symbol_name[MAX_SYMBOL_NAME_LENGTH];
	word_t instruction_length;
	word_t data_size = NULL;
	bool is_inst;
	bool is_data;
	word addr = {0};

	/* split the line by spaces */
	source_line_token = strtok_r(
			source_line_cpy,
			" \t",
			&source_line_e);

	if (is_instruction(source_line_token))
	{
		instruction_length = get_instruction_length(source_line);
		/* handle instruction */
		prog_obj->ic->data += instruction_length->data;
	}
	else if ((directive = is_directive(source_line_token)))
	{
		/* handle directive */
		handle_directive(
				source_line,
				prog_obj,
				true);
	}
	else if (strchr(source_line_token, ':')) /* is symbol */
	{
		/* in here we need to check if the symbol is followed
		 * by a directive (.entry or .extern) and ignore the symbol if so
		 */
		source_line_token = strtok_r(
				NULL,
				" ",
				&source_line_e);

		strncpy(
			symbol_name,
			source_line,
			(strchr(source_line, ':') - source_line));

		if (!strcasecmp(source_line_token, DIRECTIVE_ENTRY) ||
			!strcasecmp(source_line_token, DIRECTIVE_EXTERN))
		{
			// TODO: Write warning for unused symbol
			print_log(
				"%s Unused symbol %s at line: %d\n",
				WARNING,
				symbol_name,
				line_number);

			/* handle directive */
			handle_directive(
					strchr(source_line, ':') + 1,
					prog_obj,
					true);
		}
		else
		{

			if (!strcasecmp(source_line_token, DIRECTIVE_DATA) ||
				!strcasecmp(source_line_token, DIRECTIVE_STRING))
			{
				addr.data = prog_obj->dc->data;
				data_size = handle_directive(
								strchr(source_line, ':') + 1,
								prog_obj,
								true);

				is_inst = false;
				is_data = true;
			}
			else
			{
				is_inst = true;
				is_data = false;
				addr.data = prog_obj->ic->data;

				if (is_instruction(strchr(source_line, ':') + 1))
				{
					instruction_length = get_instruction_length(strchr(source_line, ':') + 1);
					prog_obj->ic->data += instruction_length->data;
				}
				else
				{
					print_log(
						"%s Unknown instruction %s at line: %d\n",
						ERROR,
						strchr(source_line, ':') + 1,
						line_number);

					//printf("[!] Unknown instruction: %s\n", strchr(source_line, ':') + 1);
				}
			}

			handle_symbol(
					prog_obj,
					strtok(symbol_name, " \t"),
					false,
					is_inst,
					is_data,
					data_size, /* data size */
					&addr);
		}
	}
	else
	{
		print_log(
			"%s Unfamiliar syntax at line: %d\n",
			ERROR,
			line_number);
	}

	free(source_line_cpy);
}

/* second transition is about to actually assemble the instructions
 *  - assemble instructions
 *  - replace symbols with their address (when used as operands)
 *  - create entry file (.ent) and add every .entry directive
 *  - create external file (.ext) and add every .extern directive
 */
static
void
assembler_second_transition_single_line(
		const char *source_line,
		program_object_t prog_obj,
		u_short line_number)
{
	char *source_line_cpy = strdup(source_line);
	char *source_line_e;
	char *source_line_token;
	char *directive = NULL;

	/* split the line by spaces */
	source_line_token = strtok_r(
			source_line_cpy,
			" \t",
			&source_line_e);

	if (is_instruction(source_line_token))
	{
	}
	else if ((directive = is_directive(source_line_token)))
	{
		/* handle directive, second stage */
		handle_directive(
				source_line,
				prog_obj,
				false);
	}
}

program_object_t
assembler_first_transition(
		const char *source)
{
	program_object_t prog_obj = initialize_program_object();
	symbol_table_entry_t entry;
	u_int16_t line_number = 1;
	word data_symbol_addr = {0};
	char *source_cpy = strdup(source);
	char *source_cpy_e;
	char *source_line;

	prog_obj->ic->data = 100;
	prog_obj->dc->data = 0;

	source_line = strtok_r(
			source_cpy,
			"\n",
			&source_cpy_e);

	while (source_line)
	{
		assembler_first_transition_single_line(
				source_line,
				prog_obj,
				line_number++);

		/* moving to the next source line */
		source_line = strtok_r(
				NULL,
				"\n",
				&source_cpy_e);
	}

	/* once we're done walking through the lines
	 * we need to update the addresses of the data symbols
	 */
	data_symbol_addr.data = prog_obj->ic->data;
	entry = prog_obj->sym_tbl;

	while (entry)
	{
		/* if this is a data symbol, update the address */
		if (entry->sym->is_data)
		{
			entry->sym->address.data = data_symbol_addr.data;

			/* increase the current address by the size of the data */
			data_symbol_addr.data += entry->sym->data_size.data;
		}

		/* move to the next symbol */
		entry = entry->next;
	}

	/* don't forget to free the copy of source code we made */
	free(source_cpy);
	return prog_obj;
}

bool
assembler_second_transition(
		const char *source,
		program_object_t prog_obj)
{
	u_int16_t line_number = 1;
	char *source_cpy = strdup(source);
	char *source_cpy_e;
	char *source_line;

	source_line = strtok_r(
			source_cpy,
			"\n",
			&source_cpy_e);

	while (source_line)
	{
		assembler_second_transition_single_line(
				source_line,
				prog_obj,
				line_number++);

		/* moving to the next source line */
		source_line = strtok_r(
				NULL,
				"\n",
				&source_cpy_e);
	}

	free(source_cpy);
	return false;
}
