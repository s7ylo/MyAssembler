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


static void initialize_program_object_2(program_object_t prog_obj)
{
	prog_obj->lst = symbol_table_entry_alloc();
	prog_obj->est = symbol_table_entry_alloc();
}

static program_object_t initialize_program_object(void)
{
	program_object_t prog_obj = calloc(1, sizeof(program_object));

	if (!prog_obj)
		return NULL;

	prog_obj->lst = symbol_table_entry_alloc();
	prog_obj->est = symbol_table_entry_alloc();

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

/* is_first parameter indicates whether or not
 * the guidance statement is at the first field of the line
 */
static void handle_guidance_statement(const char *source_line, bool is_first)
{
}

static void handle_symbol(const char *source_line)
{
}

u_short get_instruction_length(const char *instruction)
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

/* at the first transition we pass the code and addressing only the following
 *  - keeping a counter of the data and code sections, the code count starts from 100 and the data from 0
 *  - as each instruction passes, we increment the code section counter by the size of the instruction
 *  - as each data pass, we increment the data section counter by the size of data
 *  - symbols, we save every symbol we found (external or not) and their address (ignore external address)
 *  - we ignore the '.entry' guidance statement at the first transition
 */
void assembler_first_transition_single_line(const char *source_line, program_object_t prog_obj, word_t ic, word_t dc)
{
	char *source_line_cpy = strdup(source_line); // TODO: remember to free this at the end
	char *source_line_e;
	char *source_line_token;
	char symbol_name[30];
	u_short inst_length;
	symbol_t sym;

	/* split the line by spaces */
	source_line_token = strtok_r(
			source_line_cpy,
			" \t",
			&source_line_e);

	if (is_instruction(source_line_token))
	{
		/* handle instruction */
		//printf("%s -> instruction\n", source_line);
	}
	else if (is_guidance_statement(source_line_token))
	{
		/* handle guidance statement */
		//printf("%s -> guidance statement\n", source_line);
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
			/* handle symbol */
			sym = sym_alloc();

			// TODO: save name
			strncpy(
				symbol_name,
				source_line,
				(strchr(source_line, ':') - source_line));

			strncpy(
				sym->name,
				strtok(symbol_name, " \t"),
				strlen(symbol_name));

			/* check if symbol exist in symbol table */
			if (is_symbol_exist_in_table(prog_obj->lst, sym->name))
			{
				// TODO: symbol already exist in table, pop some error and continue
			}
			else
			{
				/* none external symbol */
				sym->is_external = false;

				/* next is the following field
				 * .data / .string statement or instruction
				 */
				if (!strcasecmp(source_line_token, STATEMENT_DATA) ||
					!strcasecmp(source_line_token, STATEMENT_STRING))
				{
					/* statement */
					/* save address as for data */
					sym->address.data = dc->data;
					sym->is_instruction = false;

					// TODO: parse the rest of the data line and increase dc appropriately
				}
				else
				{
					/* instruction */
					/* save address as for code */
					sym->address.data = ic->data;
					sym->is_instruction = true;

					// TODO: parse the rest of the instruction line and increase ic appropriately
					inst_length = get_instruction_length(strchr(source_line, ':') + 1);

					if (inst_length == -1)
					{
						// TODO: unknown instruction
					}
					else
					{
						ic->data += inst_length;
					}
				}

				/* insert symbol to symbol table */
				insert_symbol_to_table(prog_obj->lst, sym);
			}
		}
	}
}

program_object_t assembler_first_transition(const char *source)
{
	program_object_t prog_obj = initialize_program_object();
	word inst_count;
	word data_count;
	char *source_cpy = strdup(source);
	char *source_cpy_e;
	char *source_line;

	inst_count.data = 100;
	data_count.data = 0;

	source_line = strtok_r(
			source_cpy,
			"\n",
			&source_cpy_e);

	while (source_line)
	{
		assembler_first_transition_single_line(source_line, prog_obj, &inst_count, &data_count);

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
