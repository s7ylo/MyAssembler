/*
 * Assembler.c
 *
 */

#include "Assembler.h"

static void initialize_program_object(program_object_t prog_obj)
{
	prog_obj->lst = sym_table_alloc();
	prog_obj->est = sym_table_alloc();
}

program_object_t translate_source_code(const char *source_code)
{
	program_object_t program_obj = NULL;
	symbol_t sym = NULL;

	unsigned short ic = 0; /* instruction counter */
	unsigned short dc = 0; /* data counter */

	char *source_code_copy = strdup(source_code);
	char *end_source_code;

	char *line = strtok_r(source_code_copy, "\n", &end_source_code);

	char *line_copy = NULL;
	char *line_token = NULL;
	char *line_end = NULL;

	char symbol_name[30] = { 0 };
	bool is_symbol = false;

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

		/* split line by spaces and tabs */
		line_token = strtok_r(line_copy, " \t", &line_end);

		/* check if the first field (or token) is one of the following */
		if (strchr(line_token, ':'))
		{
			/* if we have found ':' this means the first field is a symbol */
		}

	next_line:
		free(line_copy);
		line = strtok_r(NULL, "\n", &end_source_code); /* move to the next line */
	}
}

void handle_instruction(const char *instruction)
{
}
