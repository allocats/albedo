#pragma once
#ifndef ALBEDO_SYMBOLS_H
#define ALBEDO_SYMBOLS_H

#include "types.h"

void init_symbol_table(SymbolTable* table, usize size, SymbolTable* parent_scope);
void insert_symbol_table(SymbolTable* table, Symbol sym);

#endif // !ALBEDO_SYMBOLS_H
