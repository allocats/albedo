#pragma once
#ifndef ALBEDO_SYMBOLS_TYPES_H
#define ALBEDO_SYMBOLS_TYPES_H

#include "../ast/types.h"
#include "../types/types.h"
#include "../utils/types.h"

#define X_SYMBOLS(X)    \
    X(S_Function)       \
    X(S_Constant)       \
    X(S_Variable)       \
    X(S_Type)           \

typedef enum {
    X_SYMBOLS(GENERATE_ENUM)
} SymbolKind;

static const char* SYMBOL_KIND_STRINGS[] = {
    X_SYMBOLS(GENERATE_STRING)
};

// maybe tagged union here, but for now just use 
// ast node? might be simpler + reduced memory overhead

typedef struct Symbol {
    SymbolKind kind;
    u32 hash;

    AstNode* ast_node;

    bool is_private;
    bool is_external;
    u32 times_called;

    struct Symbol* next;
} Symbol;

typedef struct SymbolTable {
    struct SymbolTable* next_scope;

    Symbol** symbols;
    usize symbol_count;
    usize symbol_capacity;
} SymbolTable;

#endif // !ALBEDO_SYMBOLS_TYPES_H
