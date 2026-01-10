#pragma once
#ifndef ALBEDO_AST_TYPES_H
#define ALBEDO_AST_TYPES_H

#include "../utils/types.h"

typedef struct AstNode AstNode;

#define X_AST(X)    \
    X(A_Function)   \
    X(A_Struct)     \
    X(A_Enum)       \

typedef enum {
    X_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    X_AST(GENERATE_STRING)
};

/*
*   A span into the tokens vector, the type checking pass
*   will sort out the actual types from this span
*/
typedef struct {
    u32 start_index;
    u32 end_index;
} AstTypeSpan;

/*
*   A span to store a span inside the original buffer
*/
typedef struct {
    usize start;
    usize end;
} ByteSpan;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstTypeSpan type;
} AstParam;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstParam** params;
    u8 param_count;
    u8 param_capacity;

    u32 return_type_id;

    AstNode** stmts;
    u32 stmt_count;
    u32 stmt_capacity;
} AstFunction;

typedef struct AstNode {
    AstKind kind;
    ByteSpan span;

    union {
        AstFunction function;
    };
} AstNode;

#endif // !ALBEDO_AST_TYPES_H
