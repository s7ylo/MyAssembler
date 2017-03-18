/*
 * instruction.c
 */

#include "instruction.h"

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

static reg_info available_registers[MaxReg] =
{
		{ R0, "R0" },
		{ R1, "R1" },
		{ R2, "R2" },
		{ R3, "R3" },
		{ R4, "R4" },
		{ R5, "R5" },
		{ R6, "R6" },
		{ R7, "R7" },
};

static
bool
is_operand_reg(
		const char *operand)
{
	bool is_reg = false;
	char *operand_cpy = strdup(operand);
	char *operand_cpy_e;
	char *token = strtok_r(operand_cpy, " \t,", &operand_cpy_e);
	int i;

	for (i = 0; i < REGS_COUNT; i++)
	{
		if (!strcasecmp(token, available_registers[i].name))
		{
			is_reg = true;
			break;
		}
	}

	free(operand_cpy);
	return is_reg;
}

/* this function returns one of the operand types listed in instruction.h */
static
ushort
get_operand_type(
		const char *operand)
{
	ushort type = OPERAND_ADDRESS;
	char *operand_cpy = strdup(operand);
	char *operand_cpy_e;
	char *token = strtok_r(operand_cpy, " \t,", &operand_cpy_e);

	if (token[0] == '#')
	{
		type = OPERAND_IMMEDIATE;
		goto clean_and_return;
	}
	else if (is_operand_reg(operand_cpy))
	{
		type = OPERAND_REGISTER;
		goto clean_and_return;
	}
	else if (strchr(operand_cpy, '[') &&
			 strchr(operand_cpy, ']'))
	{
		type = OPERAND_REG_INDEX;
	}

clean_and_return:
	free(operand_cpy);
	return type;
}

static
ushort
get_instruction_opcode(
		const char *inst)
{
	return 0;
}

static
static

bool
is_instruction(
		const char *field)
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

word_t
get_instruction_length(
		const char *instruction)
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
assemble_instruction(
		const char *instruction_line)
{
	assembled_instruction_t asm_inst = (assembled_instruction_t)calloc(1, sizeof(assembled_instruction));
	word_t inst_length = NULL;
	char *instruction_line_cpy = strdup(instruction_line);
	char *instruction_line_cpy_e;
	char *token;

	inst_length = get_instruction_length(instruction_line_cpy);
	asm_inst->opcode = (word**)calloc(1, sizeof(word*));

	/* based on the length we allocate the amount of words we need for the opcodes */
	*(asm_inst->opcode) = (word_t)calloc(inst_length->data, sizeof(word));
	asm_inst->length.data = inst_length->data;

	/* this is the part where we create the instruction's opcodes */
	/* the first opcode (word) characterize the entire instruction.
	 * also the first opcode's encoding is absolute */
	asm_inst->opcode[0]->data = ENCODE_ABSOLUTE;

	token = strtok_r(
			instruction_line_cpy,
			" \t,",
			&instruction_line_cpy);
	/* set the instruction opcode */
	asm_inst->opcode[0]->data |= INSTRUCTION_OPCODE(get_instruction_opcode(token));

	// TODO: Check if we are dealing with 3 opcodes length instruction but with two registers as operands (length is 2)

	/* set source operand
	 * source operand is available on 3 words length instructions
	 */
	if (inst_length->data == 3)
	{
		asm_inst->opcode[0]->data |= INSTRUCTION_SRC_OP(get_operand_type())
	}
	else /* no source operand in available, set to 0 */
	{
		asm_inst->opcode[0]->data |= INSTRUCTION_SRC_OP(0);
	}

	free(instruction_line_cpy);
	return NULL;
}
