/*
 * Assembler.h
 */

#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include "arch.h"
#include "symbols.h"
#include "directive.h"
#include "instruction.h"

#define CODE_SECTION_BASE (100)

typedef struct _section
{
	word size;
	/*implement*/
} section, *section_t;

typedef struct _program_image
{
	/* the size in words of the code length */
	word code_section_size;

	/* the size in words of the data length */
	word data_section_size;

	word code_section[1000];
	word data_section[1000];
} program_image, *program_image_t;

typedef struct _program_object
{
	/* the program's image */
	program_image prog_image;

	/* external symbol table */
	symbol_table_entry_t extsymtab_entry;
	/* the symbol table */
	symbol_table_entry_t symtab_entry;

	/* instruction and data counters */
	word_t ic;
	word_t dc;

	/* the size in words of the code section */
	word_t code_size;
	/* the size in words of the data section */
	word_t data_size;
} program_object, *program_object_t;

void
handle_symbol(
	program_object_t prog_obj,
	const char *name,
	word_t flags,
	word_t data_size,
	word_t address);

program_object_t
assembler_first_transition(const char *source);

bool
assembler_second_transition(
		const char *source,
		program_object_t prog_obj);

#endif /* ASSEMBLER_H_ */
