/*
 * symbols.c
 *
 */

#include "symbols.h"

symbol_table_entry_t
symbol_table_entry_alloc(void)
{
	symbol_table_entry_t entry = (symbol_table_entry_t)calloc(1, sizeof(symbol_table_entry));

	return entry;
}

symbol_t sym_alloc(void)
{
	symbol_t sym = (symbol_t)calloc(1, sizeof(symbol));

	return sym;
}

bool is_symbol_exist_in_table(symbol_table_entry_t sym_table, const char *sym_name)
{
	symbol_table_entry_t tail = sym_table;

	while (tail->next)
	{
		if (!strcmp(tail->sym->name, sym_name))
		{
			/* matched symbol name found, return true */
			return true;
		}

		tail = tail->next;
	}

	/* no match found */
	return false;
}

void insert_symbol_to_table(symbol_table_entry_t sym_table, symbol_t sym)
{
	symbol_table_entry_t entry = sym_table;

	/* if this is an empty list, add it at the current node */

	while (entry->next)
	{
		entry = entry->next;
	}

	entry->sym = sym;

	/* allocate new entry as the next one */
	entry->next = symbol_table_entry_alloc();
}
