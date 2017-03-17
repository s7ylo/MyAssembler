/*
 * symbols.h
 */

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

#include "arch.h"

#define MAX_SYMBOL_NAME_LENGTH (30)

typedef struct _symbol
{
	char name[MAX_SYMBOL_NAME_LENGTH];
	word address;
	bool is_external;
	bool is_instruction;
	bool is_data;
	word data_size;
} symbol, *symbol_t;

typedef struct _symbol_table_entry
{
	struct _symbol_table_entry *next;
	symbol_t sym;
} symbol_table_entry, *symbol_table_entry_t;

symbol_table_entry_t
symbol_table_entry_alloc(void);

symbol_t
sym_alloc(void);

void
sym_free(symbol_t sym);

bool
is_symbol_exist_in_table(
		symbol_table_entry_t sym_table,
		const char *sym_name);

void
insert_symbol_to_table(
		symbol_table_entry_t *head,
		symbol_t sym);

symbol_t
lookup_symbol_by_name(
		symbol_table_entry_t sym_table,
		const char *name);

#endif /* SYMBOLS_H_ */
