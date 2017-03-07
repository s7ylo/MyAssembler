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


static void initialize_program_object_2(program_object_t prog_obj)
{
	prog_obj->program_base.data = 100;
	prog_obj->lst = symbol_table_entry_alloc();
	prog_obj->est = symbol_table_entry_alloc();
}

static program_object_t initialize_program_object(void)
{
	program_object_t prog_obj = calloc(1, sizeof(program_object));

	if (!prog_obj)
		return NULL;

	prog_obj->program_base.data = 100;
	prog_obj->lst = symbol_table_entry_alloc();
	prog_obj->est = symbol_table_entry_alloc();

	return prog_obj;
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

/* this function translate an instruction from a string format
 * and populates the program object structure
 */
bool translate_instruction(const char *instruction, program_object_t prog_obj)
{
	instruction_t inst;
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
	char *source_line_cpy = strdup(source_line);
	char *source_line_e;
	char *source_line_token;

	/* split the line by spaces */
	source_line_token = strtok_r(
			source_line_cpy,
			" ",
			&source_line_e);

	if (is_instruction(source_line_token))
	{
		/* handle instruction */
	}
	else if (is_guidance_statement(source_line_token))
	{
		/* handle guidance statement */
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

		if (strstr(source_line_token, STATEMENT_ENTRY) ||
			strstr(source_line_token, STATEMENT_EXTERN))
		{
			/* go to handle guidance statement */
		}
		else
		{
			/* handle symbol */
		}
	}
}

program_object_t assembler_first_transition(const char *source)
{
	program_object_t prog_obj = initialize_program_object();
	u_short inst_count = 100;
	u_short data_count = 0;
	char *source_cpy = strdup(source);
	char *source_cpy_e;
	char *source_line;

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

	/* initializing the program object */
	program_obj = (program_object_t)calloc(1, sizeof(program_object));

	if (!program_obj)
	{
		// TODO: throw error
		return NULL;
	}

	initialize_program_object_2(program_obj);

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
			line_token = strtok_r(NULL, " \t", &line_end);

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

	//next_line:
		free(line_copy);
		memset(symbol_name, 0, sizeof(symbol_name));
		line = strtok_r(NULL, "\n", &end_source_code); /* move to the next line */
	}

	return program_obj;
}
