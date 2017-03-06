/*
 * Assembler.c
 *
 */

#include "Assembler.h"

/* all available instructions */
static char *available_instructions[AVAILABLE_INST_COUNT] =
{
		"mov",
		"cmp",
		"add",
		"sub",
		"not",
		"clr",
		"lea",
		"inc",
		"dec",
		"jmp",
		"bne",
		"red",
		"prn",
		"jsr",
		"rts",
		"stop"
};

/* all available guidance statements */
static char *available_statements[AVAILABLE_STATEMENTS_COUNT] =
{
		".data",
		".string",
		".extern",
		".entry"
};

static void initialize_program_object(program_object_t prog_obj)
{
	prog_obj->program_base.data = 100;
	prog_obj->lst = symbol_table_entry_alloc();
	prog_obj->est = symbol_table_entry_alloc();
}

static bool is_instruction(const char *field)
{
	int i;

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcmp(field, available_instructions[i]))
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
		if (!strcmp(field, available_statements[i]))
		{
			return true;
		}
	}

	return false;
}

program_object_t translate_source_code(const char *source_code)
{
	program_object_t program_obj = NULL;
	symbol_t sym = NULL;

	unsigned short current_address = 100;
	unsigned short ic = 0; /* instruction counter */
	unsigned short dc = 0; /* data counter */

	char *source_code_copy = strdup(source_code);
	char *end_source_code;

	char *line = strtok_r(source_code_copy, "\n", &end_source_code);

	char *line_copy = NULL;
	char *line_token = NULL;
	char *line_end = NULL;

	char symbol_name[30] = { 0 };

	program_obj = (program_object_t)calloc(1, sizeof(program_object));

	if (!program_obj)
	{
		// TODO: throw error
		return NULL;
	}

	initialize_program_object(program_obj);

	while (line)
	{
		/* create a copy of the current line */
		line_copy = strdup(line);

		/* split lines by spaces */
		line_token = strtok_r(line_copy, " ", &line_end);

		/* first field checking
		 * what we're handling
		 *  - instruction line (starts with an instruction)
		 *  - symbol so it should end with ':'
		 *  - guidance statement one of the following
		 *    - .entry
		 *    - .extern
		 */
		if (is_instruction(line_token))
		{
			/* handle instruction line */
		}
		else if (strchr(line_copy, ':'))
		{
			/* if this is a symbol, we need to check that the following field isn't '.extern' or '.entry'
			 * if so, just ignore the symbol and handle that as a guidance statement
			 */
			line_token = strtok_r(line_copy, " \t", &line_end);

			if (is_guidance_statement(line_token)) /* handle guidance statement */
			{
			}
			else /* handle symbol normally */
			{
				/* extract the symbol name */
				strncpy(
					symbol_name,
					line_copy,
					(strchr(line_copy, ':') - line_copy));

				/* check if the symbol already exist in the symbol table */
				if (is_symbol_exist_in_table(program_obj->lst, symbol_name))
				{
					printf("The symbol: %s, is already exist in symbol table\n", symbol_name);
				}
				else
				{
					sym = sym_alloc();

					sym->id.data = dc;
					sym->address.data = current_address;

					/* TODO: verify whether the symbol followed by an instruction (code) or data (.string or .data) */

					/* copy the name */
					strncpy(
						sym->name,
						strtok(symbol_name, " \t"),
						strlen(symbol_name));

					/* add to symbol table */
					insert_symbol_to_table(program_obj->lst, sym);
				}
			}


		}
		else if (is_guidance_statement(line_token)) /* first field is guidance statement */
		{
		}

		/* split line by spaces and tabs */
		//line_token = strtok_r(line_copy, " \t", &line_end);

	//next_line:
		free(line_copy);
		memset(symbol_name, 0, sizeof(symbol_name));
		line = strtok_r(NULL, "\n", &end_source_code); /* move to the next line */
	}

	return program_obj;
}

void handle_instruction(const char *instruction)
{
}
