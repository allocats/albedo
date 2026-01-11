#pragma once
#ifndef ALBEDO_AST_TYPES_H
#define ALBEDO_AST_TYPES_H

#include "../utils/types.h"

typedef struct AstNode AstNode;

#define X_AST(X)    \
    X(A_Module)     \
    X(A_Import)     \
                    \
    X(A_Param)      \
    X(A_Function)   \
    X(A_Struct)     \
    X(A_Enum)       \
                    \
    X(A_Static)     \
    X(A_Const)      \
    X(A_Var)        \

typedef enum {
    X_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    X_AST(GENERATE_STRING)
};

/*
*   A span into the tokens vector, the type checking pass
*   will sort out the actual types from this span
*
*   if both start_index and end_index are set to 0 
*   then there is no span present
*/
typedef struct {
    u32 start_index;
    u32 end_index;
} AstSpan;

/*
*   A span that indexes inside the original buffer
*/
typedef struct {
    usize start;
    usize end;
} ByteSpan;

typedef struct {
    char* path_ptr;
    usize path_len;
} AstModule;

typedef struct {
    char* path_ptr;
    usize path_len;
    bool is_lib;
} AstImport;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;
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

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    AstNode* default_value;
} AstStructField;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstStructField* fields;
    u16 field_conut;
    u16 field_capacity;

    AstNode** methods;
    u16 method_count;
    u16 method_capacity;
} AstStruct;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstNode* value;

    char** field_ptrs;
    usize* field_lens;
    u8 field_count;
    u8 field_capacity;
} AstEnumVariant;

typedef struct {
    char* name_ptr;
    usize name_len;

    // This is optional, it can only be integers, both signed 
    // and unsigned, BUT if this is not included in the source code 
    // then enums are treated like Rust enums, tagged enums if you will. 
    // If there is a specified integer type then it is treated like 
    // C style enums, where it is just an integer underneath
    AstSpan type;

    AstEnumVariant* variants;
    u64 variant_count;
    u64 variant_capacity;
} AstEnum;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    AstNode* value;
} AstStatic;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    // Must be initialised 
    AstNode* value;
} AstConst;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    AstNode* value;
} AstVar;

typedef struct {
    AstNode* condition;

    AstNode** stmts;
    u32 stmt_count;
    u32 stmt_capacity;

    // If pointing to block node then just "else", but if 
    // pointing to another AstIf node then it's "else if".
    // Null if no else statement
    AstNode* else_stmt;
} AstIf;

typedef struct {
    AstNode* condition;

    AstNode** stmts;
    u32 stmt_count;
    u32 stmt_capacity;
} AstWhile;

typedef struct {
    AstNode** stmts;
    u32 stmt_count;
    u32 stmt_capacity;
} AstLoop;

typedef struct {
    AstSpan temp_variables;
 
    // not sure what to call this
    AstNode* target; 

    AstNode** stmts;
    u32 stmt_count;
    u32 stmt_capacity;
} AstFor;

typedef struct AstNode {
    AstKind kind;
    ByteSpan span;

    union {
        AstImport import_decl;
        AstModule module_decl;

        AstFunction function_decl;
        AstStruct struct_decl;
        AstEnum enum_decl;

        AstStatic static_decl;
        AstConst const_decl;
        AstVar var_decl;

        AstIf if_stmt;
        AstWhile while_loop;
        AstLoop loop_loop;
        AstFor for_loop;
    };
} AstNode;

#endif // !ALBEDO_AST_TYPES_H
