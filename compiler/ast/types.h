#pragma once
#ifndef ALBEDO_AST_TYPES_H
#define ALBEDO_AST_TYPES_H

#include "../token/types.h"
#include "../utils/types.h"

typedef struct AstNode AstNode;

#define X_AST(X)        \
    X(A_Module)         \
    X(A_Import)         \
                        \
    X(A_ExternFn)       \
    X(A_ExternStruct)   \
    X(A_ExternType)     \
                        \
    X(A_Function)       \
    X(A_Struct)         \
    X(A_Enum)           \
                        \
    X(A_Block)          \
    X(A_Defer)          \
    X(A_Return)         \
                        \
    X(A_Continue)       \
    X(A_Break)          \
                        \
    X(A_Static)         \
    X(A_Const)          \
    X(A_VarDecl)        \
                        \
    X(A_If)             \
    X(A_Match)          \
    X(A_While)          \
    X(A_Loop)           \
    X(A_For)            \
                        \
    X(A_Binary)         \
    X(A_Unary)          \
    X(A_Assign)         \
    X(A_Call)           \
    X(A_MethodCall)     \
    X(A_Ident)          \
    X(A_Literal)        \
    X(A_Index)          \
    X(A_Cast)           \
    X(A_StructInit)     \

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

typedef enum {
    IMPORT_LIB,
    IMPORT_REL
} ImportKind;

typedef struct {
    char* path_ptr;
    usize path_len;
    ImportKind kind;
} AstImport;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;
} AstParam;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstParam* params;
    u16 param_count;
    u16 param_capacity;

    AstSpan return_type;

    AstNode* body;
} AstFunction;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    // Optional, null to indicate no expression 
    AstNode* default_value;
} AstField;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstField* fields;
    u16 field_conut;
    u16 field_capacity;

    AstNode** methods;
    u16 method_count;
    u16 method_capacity;
} AstStruct;

typedef struct {
    char* name_ptr;
    usize name_len;

    // Optional value
    // Example
    //
    // enum Fruit: u8 {
    //     Apple = 1;
    // }
    AstNode* value;
} AstEnumVariant;

typedef struct {
    char* name_ptr;
    usize name_len;

    // This is optional, it can only be integers, both signed 
    // and unsigned, BUT if this is not included in the source code 
    // then enums are as signed 32 bit integers.
    AstSpan type;

    AstEnumVariant* variants;
    u32 variant_count;
    u32 variant_capacity;
} AstEnum;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstField* fields;
    u32 field_count;
    u32 field_capacity;
} AstUnion;

typedef enum {
    VariantUnit,
    VariantTuple,
    VariantStruct,
} VariantKind;

typedef struct {
    char* name_ptr;
    usize name_len;

    VariantKind kind;

    union {
        struct {
            AstSpan* types;
            u8 type_count;
            u8 type_capacity;
        } tuple;

        struct {
            AstField* fields;
            u8 field_count;
            u8 field_capacity;
        } struct_fields;
    };
} AstTunionVariant;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstTunionVariant* variants;
    u32 variant_count;
    u32 variant_capacity;
} AstTunion;

typedef struct {
    char* abi_ptr;
    usize abi_len;

    char* name_ptr;
    usize name_len;

    AstParam* params;
    u8 param_count;
    u8 param_capacity;

    AstSpan return_type;

    bool is_variadic;
} AstExternFn;

typedef struct {
    char* abi_ptr;
    usize abi_len;

    char* name_ptr;
    usize name_len;

    AstField* fields;
    u16 field_count;
    u16 field_capacity;
} AstExternStruct;

typedef struct {
    char* abi_ptr;
    usize abi_len;

    char* name_ptr;
    usize name_len;
} AstExternType;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    AstNode* value;

    bool is_const;
} AstStaticDecl;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;

    // Must be initialised 
    AstNode* value;
} AstConstDecl;

typedef struct {
    char* name_ptr;
    usize name_len;

    // Optional can be inferred
    AstSpan type;

    // Optional
    AstNode* value;
} AstVarDecl;

typedef struct {
    AstNode* expr;
} AstReturn;

typedef struct {
    char* label_ptr;
    usize label_len;
} AstBreak, AstContinue;

typedef struct {
    AstNode* stmt;
} AstDefer;

typedef struct {
    AstNode* condition;

    AstNode* block;

    // If pointing to block node then just "else", but if 
    // pointing to another AstIf node then it's "else if".
    // Null if no else statement
    AstNode* else_stmt;
} AstIf;

typedef struct {
    AstNode* condition;
    AstNode* block;
} AstWhile;

typedef struct {
    AstNode* block;
} AstLoop;

typedef struct {
    char* var_ptr;
    usize var_len;

    AstNode* iterator;

    AstNode* block;
} AstFor;

typedef struct {
    char* pattern_ptr;
    usize pattern_len;

    AstNode* block;
} AstMatchArm;

typedef struct {
    AstNode* target;

    AstMatchArm* arms;
    u16 arm_count;
    u16 arm_capacity;

    AstNode* default_block;
} AstMatch;

typedef struct {
    AstNode** stmts;
    usize stmt_count;
    usize stmt_capacity;
} AstBlock;

typedef struct {
    TokenKind op;
    AstNode* left;
    AstNode* right;
} AstBinary;

typedef struct {
    TokenKind op;
    AstNode* operand;
} AstUnary;

typedef struct {
    AstNode* target;
    AstNode* value;
    TokenKind op;
} AstAssign;

typedef struct {
    char* ptr;
    usize len;
} AstIdent;

typedef struct {
    TokenKind kind;

    char* ptr;
    usize len;
} AstLiteral;

typedef struct {
    AstNode* ident;

    AstNode** args;
    u8 arg_count;
    u8 arg_capacity;
} AstFnCall;

typedef struct {
    // This should point to an ident
    AstNode* target;

    char* field_ptr;
    usize field_len;
} AstMemberAccess;

typedef struct {
    AstNode* ident;
    AstNode* index;
} AstIndex;

typedef struct {
    AstNode* expr;
    AstSpan target_type;
} AstCast;

typedef struct {
    AstNode* ident;
    AstNode** field_inits;
    u16 field_count;
    u16 field_capacity;
} AstStructInit;

typedef struct AstNode {
    AstKind kind;
    ByteSpan span;

    union {
        AstImport import_decl;
        AstModule module_decl;

        AstExternFn extern_fn;
        AstExternStruct extern_struct;
        AstExternType extern_type;

        AstFunction function_decl;
        AstStruct struct_decl;
        AstEnum enum_decl;
        AstTunion tunion_decl;
        AstUnion union_decl;

        AstBlock block;

        AstStaticDecl static_decl;
        AstConstDecl const_decl;
        AstVarDecl var_decl;

        AstDefer defer_stmt;

        AstIf if_stmt;
        AstMatch match_stmt;
        AstWhile while_loop;
        AstLoop loop_loop;
        AstFor for_loop;
        AstBreak break_stmt;
        AstContinue continue_stmt;
        AstReturn return_stmt;

        AstBinary binary_op;
        AstUnary unary_op;
        AstIdent ident;
        AstLiteral literal;
        AstFnCall fn_call;
        AstMemberAccess member_access;
        AstStructInit struct_init;
        AstIndex index_access;
        AstCast cast;
    };
} AstNode;

#endif // !ALBEDO_AST_TYPES_H
