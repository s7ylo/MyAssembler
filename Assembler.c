/*
 * Assembler.c
 *
 */

#include "Assembler.h"

static void initialize_program_object(program_object_t prog_obj)
{
	prog_obj->lst = symbol_table_entry_alloc();
	prog_obj->est = symbol_table_entry_alloc();
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

	char *sym_name = NULL;
	char *sym_end = NULL;

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

		/* check if the line contains the character ':' if so, it means that a symbol is present */
		if (strchr(line_copy, ':'))
		{
			/* extract the symbol name */
			strncpy(
				symbol_name,
				line_copy,
				(strchr(line_copy, ':') - line_copy));

			// TODO: Does that symbol already exist in the local symbol table?
			if (is_symbol_exist_in_table(program_obj->lst, symbol_name))
			{
				printf("The symbol: %s, is already exist in symbol table\n", symbol_name);
			}
			else
			{
				// TODO: otherwise, add it to the local symbol table
				sym = sym_alloc();

				sym->id.data = dc++;
				sym->type.data = SymbolTypeData;
				strncpy(sym->name, symbol_name, strlen(symbol_name));

				insert_symbol_to_table(program_obj->lst, sym);
			}
		}

		/* split line by spaces and tabs */
		//line_token = strtok_r(line_copy, " \t", &line_end);

	//next_line:
		free(line_copy);
		memset(symbol_name, 0, sizeof(symbol_name));
		line = strtok_r(NULL, "\n", &end_source_code); /* move to the next line */
	}
}

void handle_instruction(const char *instruction)
{
}
