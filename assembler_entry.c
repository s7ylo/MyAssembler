/*
 * assembler_entry.c
 */

#include "assembler_entry.h"

assembler_entry_table_entry_t
assembler_entry_table_entry_alloc(void)
{
	assembler_entry_table_entry_t entry = (assembler_entry_table_entry_t)calloc(1, sizeof(assembler_entry_table_entry));

	return entry;
}

void
assembler_entry_free(
		assembler_entry_t asm_entry)
{
	free(asm_entry);
}

assembler_entry_t
assembler_entry_alloc(void)
{
	assembler_entry_t asm_entry = (assembler_entry_t)calloc(1, sizeof(assembler_entry));

	return asm_entry;
}

void
insert_assembler_entry_to_table(
		assembler_entry_table_entry_t *head,
		assembler_entry_t asm_entry)
{
	assembler_entry_table_entry_t new_entry = assembler_entry_table_entry_alloc();
	assembler_entry_table_entry_t entry = *head;
	assembler_entry_table_entry_t last_entry = NULL;

	/* if the list is empty, add as first link and return */
	if (!entry)
	{
		new_entry->asm_entry = asm_entry;
		*head = new_entry;
		return;
	}

	while (entry)
	{
		last_entry = entry;
		entry = entry->next;
	}

	new_entry->asm_entry = asm_entry;
	last_entry->next = new_entry;
}
