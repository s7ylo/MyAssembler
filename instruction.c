/*
 * instruction.c
 */

#include "instruction.h"

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

u_short is_instruction(const char *field)
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

assembled_instruction_t
assemble_instruction(const char *instruction_line)
{
	char *instruction_line_cpy = strdup(instruction_line); // remember to free this at the end
}
