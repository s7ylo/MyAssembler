/*
 * instruction.c
 */

#include "instruction.h"
#include "symbols.h"
#include "Assembler.h"

/* all available instructions */
static instruction_info available_instructions[AVAILABLE_INST_COUNT] =
{
		{ { Mov }, "mov", { 3 } },
		{ { Cmp }, "cmp", { 3 } },
		{ { Add }, "add", { 3 } },
		{ { Sub }, "sub", { 3 } },
		{ { Lea }, "lea", { 3 } },
		{ { Not }, "not", { 2 } },
		{ { Clr }, "clr", { 2 } },
		{ { Inc }, "inc", { 2 } },
		{ { Dec }, "dec", { 2 } },
		{ { Jmp }, "jmp", { 2 } },
		{ { Bne }, "bne", { 2 } },
		{ { Red }, "red", { 2 } },
		{ { Prn }, "prn", { 2 } },
		{ { Jsr }, "jsr", { 2 } },
		{ { Rts }, "rts", { 1 } },
		{ { Stop }, "stop", { 1 } }
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
word_t
get_operand_type(
		const char *operand)
{
	word_t type = (word_t)calloc(1, sizeof(word));
	char *operand_cpy = strdup(operand);
	char *operand_cpy_e;
	char *token = strtok_r(operand_cpy, " \t,", &operand_cpy_e);

	type->data = OPERAND_ADDRESS;

	if (token[0] == '#')
	{
		type->data = OPERAND_IMMEDIATE;
		goto clean_and_return;
	}
	else if (is_operand_reg(operand_cpy))
	{
		type->data = OPERAND_REGISTER;
		goto clean_and_return;
	}
	else if (strchr(operand_cpy, '[') &&
			 strchr(operand_cpy, ']'))
	{
		type->data = OPERAND_REG_INDEX;
	}

clean_and_return:
	free(operand_cpy);
	return type;
}

static
bool
is_special_instruction(
		const char *operands)
{
	bool is_special = false;
	char *operands_cpy = strdup(operands);
	char *operands_cpy_e;
	char *token = strtok_r(
			operands_cpy,
			" \t,",
			&operands_cpy_e);

	if (get_operand_type(token)->data == OPERAND_REGISTER)
	{
		token = strtok_r(
				NULL,
				" \t,",
				&operands_cpy_e);

		if (get_operand_type(token)->data == OPERAND_REGISTER)
		{
			is_special = true;
		}
		else
		{
			is_special = false;
		}
	}

	free(operands_cpy);
	return is_special;
}

static
word_t
get_instruction_opcode(
		const char *inst)
{
	word_t opcode = NULL;
	ushort i;
	char *inst_cpy = strdup(inst);
	char *inst_cpy_e;
	char *token = strtok_r(
			inst_cpy,
			" \t",
			&inst_cpy_e);

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(token, available_instructions[i].name))
		{
			opcode = (word_t)calloc(1, sizeof(word));
			opcode->data = available_instructions[i].opcode.data;
			break;
		}
	}

	free(inst_cpy);
	return opcode;
}

static
void
assemble_single_operand(
		const char *operand,
		word_t operand_type,
		ushort i,
		assembled_instruction_t asm_inst,
		program_object_t prog_obj)
{
	char *operand_cpy = strdup(operand);
	symbol_t sym = NULL;

	switch (operand_type->data)
	{
	case OPERAND_IMMEDIATE:
		{
			asm_inst->opcode[i]->data |= ENCODE_ABSOLUTE;
		}
		break;
	case OPERAND_REGISTER:
		{
			asm_inst->opcode[i]->data |= ENCODE_ABSOLUTE;
		}
		break;
	case OPERAND_REG_INDEX:
		{
			asm_inst->opcode[i]->data |= ENCODE_ABSOLUTE;

			/* need to get the values of the outer register and the inner register separately */
		}
		break;
	case OPERAND_ADDRESS: /* this is a symbol, we use a relocatable encode */
		{
			/* before setting relocatable for a symbol
			 * we check if this is an external symbol
			 * and if it is, we set encode as external
			 */
			sym = lookup_symbol_by_name(prog_obj->sym_tbl, operand_cpy);

			if (sym->flags.data & SYMBOL_TYPE_EXTERN)
			{
				asm_inst->opcode[i]->data |= ENCODE_EXTERNAL;
			}
			else
			{
				asm_inst->opcode[i]->data |= ENCODE_RELOCATABLE;
			}
		}
		break;
	}

	free(operand_cpy);
}

static
void
assemble_operands(
		const char *operands,
		assembled_instruction_t asm_inst,
		program_object_t prog_obj)
{
	word_t operand_type = NULL;
	ushort i;
	char *operands_cpy = strdup(operands);
	char *operands_cpy_e;
	char *token;

	if (asm_inst->length.data == 3)
	{
		for (i = 1; i < 3; i++)
		{
			/* extract the next operand */
			token = strtok_r(
					operands_cpy,
					" \t,",
					&operands_cpy_e);

			operand_type = get_operand_type(token);

			assemble_single_operand(
					token,
					operand_type,
					i,
					asm_inst,
					prog_obj);
		}
	}
	else /* we handle one operand here, unless this is a 3 opcode instruction with two operands as register */
	{
	}
}

static
word_t
get_instruction_length_no_changes(
		const char *instruction)
{
	word_t len = NULL;
	ushort i;
	char *instruction_cpy = strdup(instruction);
	char *instruction_cpy_e;
	char *token;

	token = strtok_r(
			instruction_cpy,
			" \t",
			&instruction_cpy_e);

	for (i = 0; i < AVAILABLE_INST_COUNT; i++)
	{
		if (!strcasecmp(token, available_instructions[i].name))
		{
			len = (word_t)calloc(1, sizeof(word));
			len->data = available_instructions[i].length.data;
			break;
		}
	}

	free(instruction_cpy);
	return len;
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
				if (is_special_instruction(instruction_cpy_e))
				{
					length->data = 2;
				}
				else
				{
					length->data = 3;
				}
			}
			else
			{
				length->data = available_instructions[i].length.data;
			}
		}
	}

	free(instruction_cpy);
	return length;
}

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

assembled_instruction_t
assemble_instruction(
		const char *instruction_line,
		program_object_t prog_obj)
{
	assembled_instruction_t asm_inst = (assembled_instruction_t)calloc(1, sizeof(assembled_instruction));
	word_t inst_length = NULL;
	char *instruction_line_cpy = strdup(instruction_line);
	char *instruction_line_cpy_e;
	char *token, *operands;

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
			&instruction_line_cpy_e);

	/* making a copy of the operands to handle later */
	operands = strdup(instruction_line_cpy_e);

	/* set the instruction opcode */
	asm_inst->opcode[0]->data |= INSTRUCTION_OPCODE(get_instruction_opcode(token)->data);

	/* we get the instruction length again, but now without changes
	 * so that we can handle the correct number of operands to a 3 opcodes length instructions
	 * that has both operands as register
	 */
	inst_length = get_instruction_length_no_changes(instruction_line);

	/* set opcode group */
	asm_inst->opcode[0]->data |= INSTRUCTION_GROUP((inst_length->data - 1));

	/* bits 12 - 14 are unused and set to 1 */
	asm_inst->opcode[0]->data |= INSTRUCTION_UNUSED(7); /* 7 in binary is 111 */

	switch (inst_length->data)
	{
	case 1: /* no operands */
		{
			asm_inst->opcode[0]->data |= INSTRUCTION_SRC_OP(0);
			asm_inst->opcode[0]->data |= INSTRUCTION_DST_OP(0);
		}
		break;
	case 2: /* only dest operand */
		{
			token = strtok_r(
					NULL,
					" \t,",
					&instruction_line_cpy_e);

			asm_inst->opcode[0]->data |= INSTRUCTION_SRC_OP(0);
			asm_inst->opcode[0]->data |= INSTRUCTION_DST_OP(get_operand_type(token)->data);
		}
		break; /* both source and dest operands */
	case 3:
		{
			/* extract source operand */
			token = strtok_r(
					NULL,
					" \t,",
					&instruction_line_cpy_e);

			asm_inst->opcode[0]->data |= INSTRUCTION_SRC_OP(get_operand_type(token)->data);

			/* extract dest operand */
			token = strtok_r(
					NULL,
					" \t,",
					&instruction_line_cpy_e);

			asm_inst->opcode[0]->data |= INSTRUCTION_DST_OP(get_operand_type(token)->data);
		}
		break;
	}

	free(operands);
	free(instruction_line_cpy);
	return NULL;
}
