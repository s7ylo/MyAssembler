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
void
handle_external_symbol(
		const char *name,
		word_t addr,
		program_object_t prog_obj)
{
	symbol_t sym = sym_alloc();

	if (strlen(name) <= MAX_SYMBOL_NAME_LENGTH)
	{
		strcpy(sym->name, name);
		sym->address.data = addr->data + CODE_SECTION_BASE;

		insert_symbol_to_table(
				&prog_obj->extsymtab_entry,
				sym);
	}
}

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
assemble_special_instruction_operands(
		const char *operand,
		ushort i,
		assembled_instruction_t asm_inst)
{
	char *operand_cpy = strdup(operand);
	char *operand_cpy_e;
	char *source_reg, *dest_reg;
	word temp = {0};

	/* both operands are registers, so operand will be in the format of RX,RY
	 * while X and Y are the id of the register
	 */
	source_reg = strtok_r(
			operand_cpy,
			" \t,",
			&operand_cpy_e);

	dest_reg = strtok_r(
			NULL,
			" \t,",
			&operand_cpy_e);

	/* encoding is absolute */
	asm_inst->opcode[i]->data = ENCODE_ABSOLUTE;

	/* we need this casting into a bigger size */
	temp.data = source_reg[1] - 0x30; /* 0x31 is the ascii value of 0, so we subtract to get the real value of the number */
	asm_inst->opcode[i]->data |= OPERAND_SET_SRC_REG(temp.data);

	temp.data = dest_reg[1] - 0x30;
	asm_inst->opcode[i]->data |= OPERAND_SET_DST_REG(temp.data);
}

static
void
assemble_single_operand_instruction(
		const char *operand,
		word_t operand_type,
		bool is_src_op,
		ushort i,
		assembled_instruction_t asm_inst,
		program_object_t prog_obj)
{

	switch (operand_type->data)
	{
	case OPERAND_ADDRESS:
		{
			symbol_t sym = NULL;
			word external_addr = {0};

			/* this operand encoding is relocatable
			 * unless it is an external symbol
			 */
			sym = lookup_symbol_by_name(
					prog_obj->symtab_entry,
					operand);

			if (sym)
			{
				if (sym->flags.data & SYMBOL_TYPE_EXTERN)
				{
					asm_inst->opcode[i]->data = ENCODE_EXTERNAL;
					asm_inst->opcode[i]->data |= OPERAND_SET_SYM_ADDR(0);

					external_addr.data = (prog_obj->ic->data + i);

					handle_external_symbol(
							sym->name,
							&external_addr,
							prog_obj);
				}
				else
				{
					asm_inst->opcode[i]->data = ENCODE_RELOCATABLE;
					asm_inst->opcode[i]->data |= OPERAND_SET_SYM_ADDR(sym->address.data);
				}
			}
		}
		break;

	case OPERAND_IMMEDIATE:
		{
			word temp = {0};
			char *operand_cpy = strdup(operand);
			char sign;

			asm_inst->opcode[i]->data |= ENCODE_ABSOLUTE;

			/* we need to represent the immediate value in a 13 bit */
			sign = operand_cpy[1];
			if (sign == '-')
			{
				operand_cpy += 2;
				temp.data = atoi(operand_cpy);
				temp.data = ~temp.data;
				temp.data++;
				asm_inst->opcode[i]->data |= OPERAND_SET_IMMEDIATE(temp.data);
			}
			else if (sign == '+')
			{
				operand_cpy += 2;
				asm_inst->opcode[i]->data |= OPERAND_SET_IMMEDIATE(atoi(operand_cpy));
			}
			else
			{
				operand_cpy++;
				asm_inst->opcode[i]->data |= OPERAND_SET_IMMEDIATE(atoi(operand_cpy));
			}

			/* we need to go back two bytes in order to be able to proparly free this memory */
			operand_cpy -= 2;
			free(operand_cpy);
		}
		break;

	case OPERAND_REGISTER:
		{
			word temp = {0};

			/* encoding is absolute */
			asm_inst->opcode[i]->data = ENCODE_ABSOLUTE;
			temp.data = operand[1] - 0x30;

			if (is_src_op)
			{
				asm_inst->opcode[i]->data |= OPERAND_SET_SRC_REG(temp.data);
			}
			else
			{
				asm_inst->opcode[i]->data |= OPERAND_SET_DST_REG(temp.data);
			}
		}
		break;

	case OPERAND_REG_INDEX: /* format as rX[rY] */
		{
			word outer_reg = {0};
			word inner_reg = {0};

			outer_reg.data = operand[1] - 0x30;
			inner_reg.data = operand[4] - 0x30;

			/* encoding is absolute */
			asm_inst->opcode[i]->data = ENCODE_ABSOLUTE;
			asm_inst->opcode[i]->data |= OPERAND_SET_DST_REG(outer_reg.data);
			asm_inst->opcode[i]->data |= OPERAND_SET_SRC_REG(inner_reg.data);
		}
		break;
	}
}

static
void
assemble_dual_operands_instruction(
		const char *operands,
		ushort i,
		assembled_instruction_t asm_inst,
		program_object_t prog_obj)
{
	word_t operand_type = NULL;
	char *operands_cpy = strdup(operands);
	char *operands_cpy_e;
	char *token;

	/* extract the source operand */
	token = strtok_r(
			operands_cpy,
			" \t,",
			&operands_cpy_e);

	operand_type = get_operand_type(token);

	/* need to fix this! the assemble_single_operand_instruction function handles only dest operand
	 * if we pass a source reg, it will set it as dest!!!@#$!@#!@#!@#$ FIX FIX FIX!@@#!@#
	 */
	assemble_single_operand_instruction(
			token,
			operand_type,
			true,
			i,
			asm_inst,
			prog_obj);

	/* extract the dest operand */
	token = strtok_r(
			NULL,
			" \t,",
			&operands_cpy_e);

	operand_type = get_operand_type(token);

	assemble_single_operand_instruction(
			token,
			operand_type,
			false,
			++i,
			asm_inst,
			prog_obj);
}

static
void
assemble_operands(
		const char *operands,
		assembled_instruction_t asm_inst,
		program_object_t prog_obj)
{
	word_t operand_type = NULL;
	ushort i = 1;
	char *operands_cpy = strdup(operands);

	/* we have 3 different situations here
	 *  - a single operand (dest operand only)
	 *  - both source and dest operands
	 *  - both source and dest operands as registers
	 */
	if (asm_inst->special_instruction)
	{
		assemble_special_instruction_operands(
				operands_cpy,
				i,
				asm_inst);
	}
	else
	{
		if (asm_inst->length.data == 3)
		{
			assemble_dual_operands_instruction(
					operands_cpy,
					i,
					asm_inst,
					prog_obj);
		}
		else
		{
			operand_type = get_operand_type(operands_cpy);

			assemble_single_operand_instruction(
					operands_cpy,
					operand_type,
					false,
					i,
					asm_inst,
					prog_obj);
		}
	}

	free(operands_cpy);
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
	ushort i;
	char *instruction_line_cpy = strdup(instruction_line);
	char *instruction_line_cpy_e;
	char *token, *operands;

	inst_length = get_instruction_length(instruction_line_cpy);
	asm_inst->special_instruction = false;

	if (inst_length->data != get_instruction_length_no_changes(instruction_line_cpy)->data)
	{
		/* this is a special instruction */
		asm_inst->special_instruction = true;
	}

	/* allocate the number of opcode pointers */
	asm_inst->opcode = (word**)calloc(inst_length->data, sizeof(word*));

	/* based on the length we allocate the amount of words we need for the opcodes */
	for (i = 0; i < inst_length->data; i++)
	{
		asm_inst->opcode[i] = (word_t)calloc(1, sizeof(word));
	}

	asm_inst->length.data = inst_length->data;

	/* this is the part where we create the instruction's opcodes */
	/* the first opcode (word) characterize the entire instruction.
	 * also the first opcode's encoding is absolute */
	((word_t)asm_inst->opcode[0])->data = ENCODE_ABSOLUTE;

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

			assemble_operands(
					operands,
					asm_inst,
					prog_obj);
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

			assemble_operands(
					operands,
					asm_inst,
					prog_obj);
		}
		break;
	}

	free(operands);
	free(instruction_line_cpy);

	return asm_inst;
}
