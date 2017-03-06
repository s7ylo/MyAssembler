/*
 * Assembler.h
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "arch.h"
#include "symbols.h"
#include "utils.h"

#define STATEMENT_DATA   ".data"
#define STATEMENT_STRING ".string"
#define STATEMENT_ENTRY  ".entry"
#define STATEMENT_EXTERN ".extern"

#define AVAILABLE_INST_COUNT 16
#define AVAILABLE_STATEMENTS_COUNT 4

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
	/* base address */
	word program_base;

	/* the program's image */
	program_image prog_image;

	/* the local symbol table */
	symbol_table_entry_t lst;

	/* the external symbol table */
	symbol_table_entry_t est;

	/* the entries table */
	/* the externals table */
} program_object, *program_object_t;

program_object_t translate_source_code(const char *source_code);
void handle_instruction(const char *instruction);

#endif /* ASSEMBLER_H_ */
