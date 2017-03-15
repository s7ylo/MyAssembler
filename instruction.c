/*
 * instruction.c
 */

#include "instruction.h"

/* all available registers */
static char *available_registers[REGS_COUNT] =
{
		"r0",
		"r1",
		"r2",
		"r3",
		"r4",
		"r5",
		"r6",
		"r7",
};

/* all available instructions */
static instruction_info available_instructions[AVAILABLE_INST_COUNT] =
{
		{ Mov, "mov", { 3 } },
		{ Cmp, "cmp", { 3 } },
		{ Add, "add", { 3 } },
		{ Sub, "sub", { 3 } },
		{ Lea, "lea", { 3 } },
		{ Not, "not", { 2 } },
		{ Clr, "clr", { 2 } },
		{ Inc, "inc", { 2 } },
		{ Dec, "dec", { 2 } },
		{ Jmp, "jmp", { 2 } },
		{ Bne, "bne", { 2 } },
		{ Red, "red", { 2 } },
		{ Prn, "prn", { 2 } },
		{ Jsr, "jsr", { 2 } },
		{ Rts, "rts", { 1 } },
		{ Stop, "stop", { 1 } }
};

static bool is_operand_reg(const char *operand)
{
	bool is_reg = false;
	char *operand_cpy = strdup(operand);
	char *operand_cpy_e;
	char *token = strtok_r(operand_cpy, " \t,", &operand_cpy_e);
	int i;

	for (i = 0; i < REGS_COUNT; i++)
	{
		if (!strcasecmp(token, available_registers[i]))
		{
			is_reg = true;
			break;
		}
	}

	free(operand_cpy);
	return is_reg;
}

bool is_instruction(const char *field)
{
	bool is_inst = false;
	char *field_cpy = strdup(field);
	char *field_cpy_e;
	char *token;
	int i;

	token = strtok_r(field_cpy, " \t", &field_cpy_e);

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(token, available_instructions[i].name))
		{
			is_inst = true;
			break;
		}
	}

	free(field_cpy);
	return is_inst;
}

// switch strtok with strtok_r and stick to it!
word_t get_instruction_length(const char *instruction)
{
	word_t length = (word_t)calloc(1, sizeof(word));
	char *instruction_cpy = strdup(instruction);
	char *instruction_cpy_e;
	char *token;
	int i;

	token = strtok_r(instruction_cpy, " \t,", &instruction_cpy_e);

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(token, available_instructions[i].name))
		{
			if (available_instructions[i].length.data == 3)
			{
				/* if both operands register, return 2 as length */
				token = strtok_r(NULL, " \t,", &instruction_cpy_e);

				if (is_operand_reg(token))
				{
					token = strtok_r(NULL, " \t,", &instruction_cpy_e);

					if (is_operand_reg(token))
					{
						length->data = 2;
						break;
					}

					length->data = 3;
					break;
				}

				length->data = 3;
				break;
			}

			length->data = available_instructions[i].length.data;
			break;
		}
	}

	free(instruction_cpy);
	return length;
}

assembled_instruction_t
assemble_instruction(const char *instruction_line)
{
	char *instruction_line_cpy = strdup(instruction_line); // remember to free this at the end
	free(instruction_line_cpy);
	return NULL;
}
