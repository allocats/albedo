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

typedef enum __attribute__((packed)) {
    X_SYMBOLS(GENERATE_ENUM)
} SymbolKind;

static const char* SYMBOL_KIND_STRINGS[] = {
    X_SYMBOLS(GENERATE_STRING)
};

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

    u8 __padding[2];
    u32 scope_id;

    u32 name_id;
    u32 name_hash;

    AstNode* ast_node;

    union {
        struct { u32 param_count;   } fn;
        struct { u32 type_id;       } var;
        struct { u32 type_id;       } type;
        struct { u32 value_index;   } constant;
        struct { u32 field_index;   } field;
    };
} Symbol;

typedef struct SymbolTable {
    Symbol* symbols;
    u32 symbol_count;
    u32 symbol_capacity;

    u32* slots;
    u32 slot_count;
    u32 slot_capacity;

    u32* scope_stack;
    u32 scope_depth;
    u32 scope_capacity;

    u32 current_scope_index;
} SymbolTable;

#endif // !ALBEDO_SYMBOLS_TYPES_H
