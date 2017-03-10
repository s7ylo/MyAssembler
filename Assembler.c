/*
 * Assembler.c
 */

#include "Assembler.h"

/* all available instructions */
static instruction_info available_instructions[AVAILABLE_INST_COUNT] =
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

static u_short is_instruction(const char *field)
{
	char *field_cpy = strdup(field); // free this at the end
	char *token;
	int i;

	token = strtok(field_cpy, " \t");

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(token, available_instructions[i].name))
		{
			free(field_cpy);

			/* if this is a 3 words length instruction,
			 * check if both operands are register,
			 * if so, length is 2 words
			 */

			if (available_instructions[i].length == 3)
			{
			}

			return available_instructions[i].length;
		}
	}

	free(field_cpy);
	return 0;
}

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

	if (address)
		sym->address.data = address->data;
	else
		sym->address.data = 0;

	insert_symbol_to_table(prog_obj->sym_tbl, sym);
}


static void handle_data_directive(const char *directive_line, program_object_t prog_obj)
{
	char *directive_line_cpy = strdup(directive_line);
	char *directive_line_e;
	char *token;
	char *string_value = NULL;
	u_short string_length;
	u_short i;

	/* extract the directive (.string or .data) */
	token = strtok_r(
			directive_line_cpy,
			" \t",
			&directive_line_e);

	if (!strcasecmp(token, DIRECTIVE_DATA))
	{
		token = strtok_r(
				directive_line_cpy,
				" \t,",
				&directive_line_e);

		while (token)
		{
			prog_obj->prog_image.data_image[prog_obj->prog_image.data_image_length.data++].data = strtoul(token, NULL, 10);

			/* increase data counter by one */
			prog_obj->dc->data++;

			/* move to the next number (in case it has another one) */
			token = strtok_r(
					directive_line_cpy,
					" \t,",
					&directive_line_e);
		}
	}
	else /* DIRECTIVE_STRING */
	{
		string_length = (strrchr(directive_line, '"') - strchr(directive_line, '"')) - 1;
		string_value = strchr(directive_line, '"') + 1;

		for (i = 0; i < string_length; i++)
		{
			prog_obj->prog_image.data_image[prog_obj->prog_image.data_image_length.data++].data = string_value[i];
		}

		/* set the null terminator */
		prog_obj->prog_image.data_image[prog_obj->prog_image.data_image_length.data++].data = 0;
		prog_obj->dc->data += string_length + 1;
	}
}

/* handle guidance statements
 * in first transition, the .entry statement is ignored
 */
static u_short handle_directive(const char *directive_line, program_object_t prog_obj, bool is_first_transition)
{
	char *directive_line_cpy = strdup(directive_line); // TODO: remember to free this at the end
	char *directive_line_e;
	char *token;

	token = strtok_r(
			directive_line_cpy,
			" \t",
			&directive_line_e);

	if (is_first_transition)
	{
		/* in the first transition we ignore the '.entry' statement */
		if (!strcasecmp(token, DIRECTIVE_EXTERN))
		{
			token = strtok_r(
					NULL,
					" \t",
					&directive_line_e);

			handle_symbol(
					prog_obj,
					token,
					true,
					false,
					0);

			return 0;
		}
		else if (!strcasecmp(token, DIRECTIVE_STRING) ||
				  !strcasecmp(token, DIRECTIVE_DATA))
		{
			handle_data_directive(directive_line, prog_obj);
		}
	}
	else
	{
		// TODO: Shouldn't be 0
		return 0;
	}

	// TODO: Shouldn't be 0
	return 0;
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
	char *directive;
	char symbol_name[30];
	u_short inst_length;
	bool is_inst;
	word addr = {0};

	/* split the line by spaces */
	source_line_token = strtok_r(
			source_line_cpy,
			" \t",
			&source_line_e);

	if ((inst_length = is_instruction_internal(source_line_token)))
	{
		/* handle instruction */
		prog_obj->ic->data += inst_length;
	}
	else if ((directive = is_directive(source_line_token)))
	{
		/* handle guidance statement */
		handle_directive(source_line, prog_obj, true);
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

		if (!strcasecmp(source_line_token, DIRECTIVE_ENTRY) ||
			!strcasecmp(source_line_token, DIRECTIVE_EXTERN))
		{
			/* handle directive */
		}
		else
		{
			strncpy(
				symbol_name,
				source_line,
				(strchr(source_line, ':') - source_line));

			if (!strcasecmp(source_line_token, DIRECTIVE_DATA) ||
				!strcasecmp(source_line_token, DIRECTIVE_STRING))
			{
				handle_directive(
						strchr(source_line, ':') + 1,
						prog_obj,
						true);

				is_inst = false;
				addr.data = prog_obj->dc->data;
			}
			else
			{
				is_inst = true;
				addr.data = prog_obj->ic->data;

				if ((inst_length = is_instruction_internal(strchr(source_line, ':') + 1)))
				{
					prog_obj->ic->data += inst_length;
				}
				else
				{
					printf("[!] Unknown instruction: %s\n", strchr(source_line, ':') + 1);
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
