/*
 * instruction.h
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

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

enum InstructionIdentifier
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

enum OperandType
{
	Immediate = 0,
	Address,
	RegIndex,
	Register
};

typedef struct _instruction_info
{
	u_short identifier;
	char name[5];
	word length;
} instruction_info, *instruction_info_t;

typedef struct _assembled_instruction
{
	/* size of instruction, in words */
	word length;

	/* pointer to an array of words that represent the opcode of the assembled instruction */
	word **opcode;

} assembled_instruction, *assembled_instruction_t;

bool is_instruction(const char *field);
word_t get_instruction_length(const char *instruction);

assembled_instruction_t
assemble_instruction(const char *instruction_line);

#endif /* INSTRUCTION_H_ */
