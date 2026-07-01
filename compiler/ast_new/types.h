#pragma once
#ifndef NEW_ALBEDO_AST_TYPES_H
#define NEW_ALBEDO_AST_TYPES_H

#include "../token/types.h"
#include "../type_system/types.h"
#include "../utils/types.h"

#define IS_RESOLVED  (1 << 0)
#define IS_TYPED     (1 << 1)
#define IS_LVALUE    (1 << 2)
#define IS_CONST     (1 << 3)
#define IS_COMP_TIME (1 << 4) // TODO: Add this later down the line, just putting here so long :3
#define IS_INLINE    (1 << 5)
#define IS_EXTERNAL  (1 << 6)
#define IS_STATIC    (1 << 7)
#define IS_VARIADAC  (1 << 8)

typedef struct AstNode AstNode;
typedef struct AstTypeExpr AstTypeExpr;

#define X_AST(X)        \
    X(A_ParseError)     \
                        \
    X(A_TypeExpr)       \
                        \
    X(A_Import)         \
                        \
    X(A_Field)          \
    X(A_Parameter)      \
                        \
    X(A_Function)       \
    X(A_Struct)         \
    X(A_Enum)           \
                        \
    X(A_Union)          \
                        \
    X(A_Block)          \
    X(A_Defer)          \
    X(A_Return)         \
                        \
    X(A_Continue)       \
    X(A_Break)          \
                        \
    X(A_VarDecl)        \
                        \
    X(A_If)             \
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
    X(A_MemberAccess)   \
    X(A_Cast)           \
    X(A_StructInit)     \

typedef enum __attribute__((packed)) {
    X_AST(GENERATE_ENUM)
} AstKind;

static const char* AST_KIND_STRINGS[] = {
    X_AST(GENERATE_STRING)
};

typedef struct {
    u32 start;
    u32 end;
} Span;

typedef enum {
    A_Ty_Base,
    A_Ty_Array,
    A_Ty_Pointer,
    A_Ty_Slice,
} AstTypeExprKind;

typedef struct AstTypeExpr {
    AstTypeExprKind kind;
    bool is_constant;

    TypeEntry* entry;

    union {
        struct {
            char* ptr;
            usize len;
        } base;

        struct {
            AstTypeExpr* base_type;
            AstNode* size_expr;
        } array;

        struct {
            u32 pointer_depth;
            AstTypeExpr* pointee;
        } pointer;

        struct {
            AstTypeExpr* base_type;
        } slice;
    };
} AstTypeExpr;

typedef struct {
    char* ptr;
    usize len;
} AstImport;

typedef struct {
    char* ptr;
    usize len;

    AstNode* type;
} AstParameter;

// is_extern or is_variadac now lives in flags in AstNode!
typedef struct {
    char* ptr;
    usize len;

    AstNode* params;
    u32 param_count;
    u32 param_capacity;

    AstNode* type;

    AstNode* body;
} AstFunction;

typedef struct {
    char* ptr;
    usize len;

    AstNode* type;

    // Optional initializing value, null means no expression 
    AstNode* default_value;
} AstField;

typedef struct {
    char* ptr;
    usize len;

    AstNode* fields;
    u32 field_count;
    u32 field_capacity;
} AstStruct;

typedef struct {
    char* ptr;
    usize len;

    // Optionally user set value
    // Example:
    //
    // enum Fruit: u32 {
    //     Apple = 1;
    // }
    AstNode* value;
} AstEnumVariant;

typedef struct {
    char* ptr;
    usize len;

    // Can only be integers for now until I figure out compile time execution
    // Example: enum Fruit: u8 {
    AstNode* type;

    AstNode* variants;
    u32 variant_count;
    u32 variant_capacity;
} AstEnum;

typedef struct {
    char* ptr;
    usize len;

    AstNode* fields;
    u32 field_count;
    u32 field_capacity;
} AstUnion;

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
    char* ptr;
    usize len;

    AstNode** args;
    u32 arg_count;
    u32 arg_capacity;
} AstCall;

typedef struct {
    AstNode* target;

    char* field_ptr;
    usize field_len;
} AstMemberAccess;

typedef struct {
    AstNode* ident;
    AstNode* index;
} AstIndex;

typedef struct {
    AstNode* target;

    AstNode* type;
} AstCast;

typedef struct {
    char* ptr;
    usize len;

    AstNode* value;
} AstFieldInit;

typedef struct {
    char* ptr;
    usize len;

    AstFieldInit* fields;
    u32 field_count;
    u32 field_capacity;
} AstStructInit;

typedef struct {
    char* ptr;
    usize len;

    // Not optional can NOT be inferred
    AstNode* type;

    // Optional if not const
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
    // points to node of A_Block kind
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
    AstNode* iterator;
    AstNode* condition;
    AstNode* step;

    AstNode* block;
} AstFor;

typedef struct {
    AstNode** stmts;
    u32 stmt_count;
    u32 stmt_capacity;
} AstBlock;

typedef struct AstNode {
    AstKind kind;
    u16 flags;
    u32 file_index;

    Span span;

    void* resolved_symbol;

    union {
        AstImport import_decl;
        AstStruct struct_decl;
        AstEnum enum_decl;
        AstUnion union_decl;
        AstFunction function_decl;

        AstParameter parameter;
        AstField field;
        AstEnumVariant enum_variant;

        AstCall call;

        AstIdent identifier;
        AstLiteral literal;

        AstVarDecl var_decl;
        AstAssign assign;
        AstCast cast;

        AstBinary binary_op;
        AstUnary unary_op;

        AstIndex index;
        AstMemberAccess member_access;
        AstStructInit struct_init;

        AstReturn return_stmt;
        AstDefer defer_stmt;
        AstBreak break_stmt;
        AstContinue continue_stmt;

        AstIf if_stmt;

        AstLoop infinite_loop;
        AstWhile while_loop;
        AstFor for_loop;

        AstBlock block;

        AstTypeExpr type_expr;
    };
} AstNode;

typedef struct {
    AstNode** nodes;
    usize count;
    usize capacity;

    // Going to use this a checkpoint marker
    usize processed;
} AST;

#endif // !NEW_ALBEDO_AST_TYPES_H
