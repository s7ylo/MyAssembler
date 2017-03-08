/*
 * Assembler.c
 *
 */

#include "Assembler.h"

/* all available instructions */
static instruction_info available_instructions[] =
{
		{ Mov, "mov", 3 },
		{ Cmp, "cmp", 3 },
		{ Add, "add", 3 },
		{ Sub, "sub", 3 },
		{ Lea, "lea", 3 },
		{ Not, "not", 2 },
		{ Clr, "clr", 2 },
		{ Inc, "inc", 2 },
		{ Dec, "dec", 2 },
		{ Jmp, "jmp", 2 },
		{ Bne, "bne", 2 },
		{ Red, "red", 2 },
		{ Prn, "prn", 2 },
		{ Jsr, "jsr", 2 },
		{ Rts, "rts", 1 },
		{ Stop, "stop", 1 }
};

/* all available guidance statements */
static char *available_statements[AVAILABLE_STATEMENTS_COUNT] =
{
		".data",
		".string",
		".extern",
		".entry"
};

static program_object_t initialize_program_object(void)
{
	program_object_t prog_obj = calloc(1, sizeof(program_object));

	if (!prog_obj)
		return NULL;

	prog_obj->sym_tbl = symbol_table_entry_alloc();
	prog_obj->ic = (word_t)calloc(1, sizeof(word));
	prog_obj->dc = (word_t)calloc(1, sizeof(word));

	return prog_obj;
}

static bool is_instruction(const char *field)
{
	int i;

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(field, available_instructions[i].name))
		{
			return true;
		}
	}

	return false;
}

static bool is_guidance_statement(const char *field)
{
	int i;

	for (i = 0; i < AVAILABLE_STATEMENTS_COUNT; i++)
	{
		if (!strcasecmp(field, available_statements[i]))
		{
			return true;
		}
	}

	return false;
}

// TODO: Create a generic function to handle new symbols, if they exist, if length exceeds max, external, etc . .
static void handle_symbol(program_object_t prog_obj, const char *name, bool external, bool inst, word_t address)
{
	symbol_t sym;

	if (strlen(name) > MAX_SYMBOL_NAME_LENGTH)
	{
		printf("[-] Symbol: %s name length exceeds the max length\n", name);
		return;
	}

	if (is_symbol_exist_in_table(prog_obj->sym_tbl, name))
	{
		printf("[-] Symbol: %s, is already exist in the symbol table\n", name);
		return;
	}

	sym = sym_alloc();

	strncpy(
		sym->name,
		name,
		strlen(name));

	sym->is_external = external;
	sym->is_instruction = inst;
	sym->address.data = address->data;

	insert_symbol_to_table(prog_obj->sym_tbl, sym);
}

/* return the appropriate instruction length */
static u_short get_instruction_length(const char *instruction)
{
	char *instruction_cpy = strdup(instruction);
	char *token = strtok(instruction_cpy, " \t");
	int i;

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(token, available_instructions[i].name))
		{
			free(instruction_cpy);
			return available_instructions[i].length;
		}
	}

	free(instruction_cpy);
	return -1; /* unknown length */
}

/* handle guidance statements
 * in first transition, the .entry statement is ignored
 */
static u_short handle_guidance_statement(const char *source_line, program_object_t prog_obj, bool is_first_transition)
{
	char *source_line_cpy = strdup(source_line); // TODO: remember to free this at the end
	char *source_line_e;
	char *token;

	token = strtok_r(
			source_line_cpy,
			" \t",
			&source_line_e);

	if (is_first_transition)
	{
		/* in the first transition we ignore the '.entry' statement */
		if (!strcasecmp(token, STATEMENT_EXTERN))
		{
			token = strtok_r(
					NULL,
					" \t",
					&source_line_e);

			handle_symbol(
					prog_obj,
					token,
					true,
					false,
					0);

			return 0;
		}
	}
	else
	{
	}
}


/* at the first transition we pass the code and addressing only the following
 *  - keeping a counter of the data and code sections, the code count starts from 100 and the data from 0
 *  - as each instruction passes, we increment the code section counter by the size of the instruction
 *  - as each data pass, we increment the data section counter by the size of data
 *  - symbols, we save every symbol we found (external or not) and their address (ignore external address)
 *  - we ignore the '.entry' guidance statement at the first transition
 */
void assembler_first_transition_single_line(const char *source_line, program_object_t prog_obj)
{
	char *source_line_cpy = strdup(source_line); // TODO: remember to free this at the end
	char *source_line_e;
	char *source_line_token;
	char symbol_name[30];
	bool is_inst;
	u_short inst_length;
	word addr = {0};
	symbol_t sym;

	/* split the line by spaces */
	source_line_token = strtok_r(
			source_line_cpy,
			" \t",
			&source_line_e);

	if (is_instruction(source_line_token))
	{
		/* handle instruction */
		inst_length = get_instruction_length(source_line);

		if (inst_length == -1)
		{
			printf("[!] Unknown instruction: %s\n", source_line_token);
		}
		else
		{
			prog_obj->ic->data += inst_length;
		}
	}
	else if (is_guidance_statement(source_line_token))
	{
		/* handle guidance statement */
		handle_guidance_statement(source_line, prog_obj, true);
	}
	else if (strchr(source_line_token, ':')) /* is symbol */
	{
		/* in here we need to check if the symbol is followed
		 * by a guidance statement (.entry or .extern) and ignore the symbol if so
		 */
		source_line_token = strtok_r(
				NULL,
				" ",
				&source_line_e);

		if (!strcasecmp(source_line_token, STATEMENT_ENTRY) ||
			!strcasecmp(source_line_token, STATEMENT_EXTERN))
		{
			/* go to handle guidance statement */
			//printf("%s -> symbol ignored with guidance statement\n", source_line);
		}
		else
		{
			strncpy(
				symbol_name,
				source_line,
				(strchr(source_line, ':') - source_line));

			if (!strcasecmp(source_line_token, STATEMENT_DATA) ||
				!strcasecmp(source_line_token, STATEMENT_STRING))
			{
				is_inst = false;
				addr.data = prog_obj->dc->data;
			}
			else
			{
				is_inst = true;
				addr.data = prog_obj->ic->data;
				inst_length = get_instruction_length(strchr(source_line, ':') + 1);

				if (inst_length == -1)
				{
					printf("[!] Unknown instruction: %s\n", strchr(source_line, ':') + 1);
				}
				else
				{
					prog_obj->ic->data += inst_length;
				}
			}

			handle_symbol(
					prog_obj,
					strtok(symbol_name, " \t"),
					false,
					is_inst,
					&addr);

		}
	}
}

program_object_t assembler_first_transition(const char *source)
{
	program_object_t prog_obj = initialize_program_object();
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
		assembler_first_transition_single_line(source_line, prog_obj);

		/* moving to the next source line */
		source_line = strtok_r(
				NULL,
				"\n",
				&source_cpy_e);
	}

	/* don't forget to free the copy of source code we made */
	free(source_cpy);

	return NULL;
}
