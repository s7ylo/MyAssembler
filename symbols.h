/*
 * symbols.h
 */

#ifndef SYMBOLS_H_
#define SYMBOLS_H_

#include "arch.h"

#define MAX_SYMBOL_NAME_LENGTH 30

/* symbols types */
enum SymbolType
{
	SymbolTypeData = 0,
	SymbolTypeCode,
	SymbolTypeEntry,
	SymbolTypeExtern
};

typedef struct _symbol
{
	char name[MAX_SYMBOL_NAME_LENGTH];
	word id;
	word type;
	word address;
} symbol, *symbol_t;

typedef struct _list_symbol_item
{
	struct _list_symbol_item *next;
	symbol_t sym;
} list_symbol_item, *list_symbol_item_t;

/* allocate new symbol */
list_symbol_item_t sym_table_alloc(void);
symbol_t sym_alloc(void);

bool is_symbol_exist_in_table(list_symbol_item_t sym_table, const char *sym_name);
void insert_symbol_to_table(list_symbol_item_t sym_table, symbol_t sym);

#endif /* SYMBOLS_H_ */
