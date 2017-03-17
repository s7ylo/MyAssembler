/*
 * entry.h
 */

#ifndef ENTRY_H_
#define ENTRY_H_

#include "symbols.h"

typedef struct _assembler_entry
{
	char entry_name[MAX_SYMBOL_NAME_LENGTH];
	word entry_addr;
} assembler_entry, *assembler_entry_t;

typedef struct _assembler_entry_table_entry
{
	struct _assembler_entry_table_entry *next;
	assembler_entry_t asm_entry;
} assembler_entry_table_entry, *assembler_entry_table_entry_t;

assembler_entry_table_entry_t
assembler_entry_table_entry_alloc(void);

assembler_entry_t
assembler_entry_alloc(void);

void
assembler_entry_free(
		assembler_entry_t asm_entry);

void
insert_assembler_entry_to_table(
		assembler_entry_table_entry_t *head,
		assembler_entry_t asm_entry);

#endif /* ENTRY_H_ */
