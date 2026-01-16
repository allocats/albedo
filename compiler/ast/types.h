#pragma once
#ifndef ALBEDO_AST_TYPES_H
#define ALBEDO_AST_TYPES_H

#include "../token/types.h"
#include "../utils/types.h"

typedef struct AstNode AstNode;

#define X_AST(X)        \
    X(A_ParseError)     \
                        \
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
    X(A_Tunion)         \
    X(A_Union)          \
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
    char* ptr;
    usize len;
} AstSegment;

typedef struct {
    AstSegment* segments;
    u32 segment_count;
    u32 segment_capacity;
} AstModule;

// lib kind will check the user's libs directory for the module 
// rel is a relative file to the current file
typedef enum {
    ImportLib,
    ImportRel 
} ImportKind;

// if the import is a T_StrLit then it is relative
// else if it is segments (e.g: std::io) then it 
// is a library import
typedef struct {
    ImportKind kind;

    union {
        struct {
            char* ptr;
            usize len;
        } relative;

        struct {
            AstSegment* segments;
            u32 segment_count;
            u32 segment_capacity;
        } lib;
    };
} AstImport;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstNode* block;
} AstTest;

typedef struct {
    char* name_ptr;
    usize name_len;
} AstGenericParam;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstSpan type;
} AstParam;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstGenericParam* generics;
    u32 generic_count;
    u32 generic_capacity;

    AstParam* params;
    u32 param_count;
    u32 param_capacity;

    AstSpan return_type;

    AstNode* body;
} AstFunction;

typedef enum {
    FieldBasic,
    FieldStruct,
    FieldUnion,
} FieldKind;

typedef struct {
    char* name_ptr;
    usize name_len;

    FieldKind kind;

    union {
        AstSpan  basic_type;
        AstNode* struct_decl;
        AstNode* union_decl;
    };

    // Optional, null to indicate no expression 
    AstNode* default_value;
} AstField;

typedef struct {
    char* name_ptr;
    usize name_len;

    AstGenericParam* generics;
    u32 generic_count;
    u32 generic_capacity;

    AstField* fields;
    u32 field_count;
    u32 field_capacity;
} AstStruct;

typedef struct {
    char* name_ptr;
    usize name_len;

    // Optional value
    // Example
    //
    // enum Fruit: u32 {
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
    VariantUnion,
} VariantKind;

typedef struct {
    char* name_ptr;
    usize name_len;

    VariantKind kind;

    union {
        struct {
            AstSpan* types;
            u32 type_count;
            u32 type_capacity;
        } tuple;

        struct {
            AstField* fields;
            u32 field_count;
            u32 field_capacity;
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
    u32 param_count;
    u32 param_capacity;

    AstSpan return_type;

    bool is_variadic;
} AstExternFn;

typedef struct {
    char* abi_ptr;
    usize abi_len;

    char* name_ptr;
    usize name_len;

    AstField* fields;
    u32 field_count;
    u32 field_capacity;
} AstExternStruct;

typedef struct {
    char* abi_ptr;
    usize abi_len;

    char* name_ptr;
    usize name_len;
} AstExternType;

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
    u32 arg_count;
    u32 arg_capacity;
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
    char* ptr;
    usize len;

    AstNode* value;
} AstFieldInit;

typedef struct {
    AstNode* ident;

    AstFieldInit* field_inits;
    u32 field_count;
    u32 field_capacity;
} AstStructInit;

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
} AstIfBranch;

typedef struct {
    AstIfBranch* branches;
    u32 branch_count;
    u32 branch_capacity;

    // Null if no else statement
    AstNode* else_block;
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

typedef enum {
    PatternIdent,
    PatternLit,
    PatternWildcard
} PatternKind;

typedef struct {
    PatternKind kind;
    ByteSpan span;

    union {
        AstIdent ident;
        AstLiteral lit;
    };

    AstNode* block;
} AstMatchArm;

typedef struct {
    AstNode* target;

    AstMatchArm* arms;
    u32 arm_count;
    u32 arm_capacity;

    AstNode* default_block;
} AstMatch;

typedef struct {
    AstNode** stmts;
    usize stmt_count;
    usize stmt_capacity;
} AstBlock;

typedef struct AstNode {
    AstKind kind;
    ByteSpan span;

    union {
        AstImport import_decl;
        AstModule module_decl;

        AstTest test_decl;

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
