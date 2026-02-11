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

typedef struct SymbolTable SymbolTable;
typedef struct Symbol Symbol;

typedef struct {
    bool is_extern : 1;
    bool is_inline : 1;
    bool is_static : 1;
} SymbolFlags;

typedef struct Symbol {
    SymbolFlags flags;
    SymbolKind kind;
    u32 hash;

    AstNode* ast_node;

    Symbol* next;
} Symbol;

typedef struct SymbolTable {
    SymbolTable* next_scope;

    Symbol** symbols;
    usize count;
    usize capacity;
} SymbolTable;

#endif // !ALBEDO_SYMBOLS_TYPES_H
