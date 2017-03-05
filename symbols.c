/*
 * symbols.c
 *
 */

#include "symbols.h"

list_symbol_item_t sym_table_alloc(void)
{
	list_symbol_item_t sym_table = (list_symbol_item_t)calloc(1, sizeof(list_symbol_item));

	return sym_table;
}

symbol_t sym_alloc(void)
{
	symbol_t sym = (symbol_t)calloc(1, sizeof(symbol));

	return sym;
}

bool is_symbol_exist_in_table(list_symbol_item_t sym_table, const char *sym_name)
{
	list_symbol_item_t tail = sym_table;

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

void insert_symbol_to_table(list_symbol_item_t sym_table, symbol_t sym)
{
	list_symbol_item_t tail = sym_table;

	while (tail->next)
	{
		tail = tail->next;
	}

	tail->next = sym;
}
