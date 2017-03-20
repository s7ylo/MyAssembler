/*
 * instruction.h
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "Assembler.h"
#include "arch.h"

#define AVAILABLE_INST_COUNT  (16)

/* encodings */
#define ENCODE_ABSOLUTE       (0)
#define ENCODE_EXTERNAL       (1)
#define ENCODE_RELOCATABLE    (2)

/* Macros */
#define INSTRUCTION_DST_OP(x) (x << 2)
#define INSTRUCTION_SRC_OP(x) (x << 4)
#define INSTRUCTION_OPCODE(x) (x << 6)
#define INSTRUCTION_GROUP(x)  (x << 10)
#define INSTRUCTION_UNUSED(x) (x << 12)

/* Operand types */
#define OPERAND_IMMEDIATE     (0)
#define OPERAND_ADDRESS       (1)
#define OPERAND_REG_INDEX     (2)
#define OPERAND_REGISTER      (3)

enum InstructionId
{
	Mov = 0,
	Cmp,
	Add,
	Sub,
	Lea,
	Not,
	Clr,
	Inc,
	Dec,
	Jmp,
	Bne,
	Red,
	Prn,
	Jsr,
	Rts,
	Stop
};

/* Fix external types */
typedef struct _program_object *program_object_t;

typedef struct _instruction_info
{
	word opcode;
	char name[5];
	word length;
} instruction_info, *instruction_info_t;

typedef struct _reg_info
{
	ushort id;
	char name[3];
} reg_info, *reg_info_t;

typedef struct _assembled_instruction
{
	/* size of instruction, in words */
	word length;

	/* pointer to an array of words that represent the opcode of the assembled instruction */
	word **opcode;

	/* if the special instruction field is True
	 * it means that this is a 3 opcodes instruction
	 * with length of 2 because both operands are registers
	 */
	bool special_instruction;

} assembled_instruction, *assembled_instruction_t;

bool
is_instruction(
		const char *field);

word_t
get_instruction_length(
		const char *instruction);

assembled_instruction_t
assemble_instruction(
		const char *instruction_line,
		program_object_t prog_obj);

#endif /* INSTRUCTION_H_ */
