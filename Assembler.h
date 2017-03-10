/*
 * Assembler.h
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "arch.h"
#include "symbols.h"
#include "directive.h"
#include "instruction.h"

#define AVAILABLE_INST_COUNT (16)



typedef struct _instruction
{
	/* instruction length in words */
	word length;

	word w1;
	word w2;
	word w3;
} instruction, *instruction_t;

typedef struct _program_image
{
	/* the size in words of the code length */
	word code_image_length;

	/* the size in words of the data length */
	word data_image_length;

	word code_image[1000];
	word data_image[1000];
} program_image, *program_image_t;

typedef struct _program_object
{
	/* the program's image */
	program_image prog_image;

	/* the symbol table */
	symbol_table_entry_t sym_tbl;

	/* instruction and data counters */
	word_t ic;
	word_t dc;
} program_object, *program_object_t;

typedef struct _source_line_info
{
	/* how many words to increase the instruction counter */
	word ic;

	/* how many words to increase the data counter */
	word dc;
} source_line_info, *source_line_info_t;

program_object_t assembler_first_transition(const char *source);
program_object_t translate_source_code(const char *source_code);

#endif /* ASSEMBLER_H_ */
