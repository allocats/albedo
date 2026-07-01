#pragma once
#ifndef ALBEDO_SYMBOL_TYPES_H
#define ALBEDO_SYMBOL_TYPES_H

#include "../ast_new/types.h"
#include "../utils/types.h"

// Thanks C standard :3
typedef struct SymbolTable SymbolTable;

#define X_SYMBOLS(X)    \
    X(S_Function)       \
    X(S_Parameter)      \
    X(S_Constant)       \
    X(S_Variable)       \
    X(S_TypeDecl)       \

typedef enum __attribute__((packed)) {
    X_SYMBOLS(GENERATE_ENUM)
} SymbolKind;

static const char* SYMBOL_KIND_STRINGS[] = {
    X_SYMBOLS(GENERATE_STRING)
};

typedef struct {
    bool is_extern : 1;
    bool is_inline : 1;
    bool is_static : 1;
    bool is_const  : 1;
} SymbolFlags;

typedef struct {
    AstNode* ast_node;

    SymbolKind kind;
    SymbolFlags flags;

    u32 hash;

    union {
        struct {
            u32 index;
        } param;
    } as;
} Symbol;

typedef struct SymbolTable {
    Symbol* symbols;
    u32 symbol_count;
    u32 symbol_capacity;

    u32* slots;
    u32 slot_capacity;

    SymbolTable* parent_scope;
} SymbolTable;

#endif // !ALBEDO_SYMBOL_TYPES_H
