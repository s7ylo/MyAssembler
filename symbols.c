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

void
sym_free(
		symbol_t sym)
{
	free(sym);
}

symbol_t
sym_alloc(void)
{
	symbol_t sym = (symbol_t)calloc(1, sizeof(symbol));

	return sym;
}

bool
is_symbol_exist_in_table(
		symbol_table_entry_t sym_table,
		const char *sym_name)
{
	symbol_table_entry_t entry = sym_table;

	while (entry)
	{
		if (!strcasecmp(entry->sym->name, sym_name))
		{
			return true;
		}

		entry = entry->next;
	}

	return false;
}

void
insert_symbol_to_table(
		symbol_table_entry_t *head,
		symbol_t sym)
{
	symbol_table_entry_t new_entry = symbol_table_entry_alloc();
	symbol_table_entry_t entry = *head;
	symbol_table_entry_t last_entry = NULL;

	/* if the list is empty, add as first link and return */
	if (!entry)
	{
		new_entry->sym = sym;
		*head = new_entry;
		return;
	}

	while (entry)
	{
		last_entry = entry;
		entry = entry->next;
	}

	new_entry->sym = sym;
	last_entry->next = new_entry;
}

symbol_t
lookup_symbol_by_name(
		symbol_table_entry_t sym_table,
		const char *name)
{
	symbol_table_entry_t entry = sym_table;

	while (entry)
	{
		if (!strcasecmp(entry->sym->name, name))
		{
			return entry->sym;
		}

		entry = entry->next;
	}

	return NULL;
}
