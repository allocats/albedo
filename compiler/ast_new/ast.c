#include "ast.h"

#include "../albedo/types.h"
#include "types.h"

extern AlbedoCtx albedo_ctx;

#define PARAM_INIT_CAP   4
#define ARG_INIT_CAP     4
#define FIELD_INIT_CAP   8
#define VARIANT_INIT_CAP 8
#define BRANCH_INIT_CAP  4
#define BLOCK_INIT_CAP   8

/* 
*  ----------------------------
*  | 1. Node making functions |
*  ----------------------------
*/ 
AstNode* ast_make_generic_blank_node(AstKind kind, u16 flags, u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = kind;
    node -> flags = flags;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    return node;
}

AstNode* ast_make_function_node(u16 flags, u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Function;
    node -> flags = flags;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> function_decl.params = arena_alloc(albedo_ctx.arena, sizeof(AstNode) * PARAM_INIT_CAP);
    node -> function_decl.param_count = 0;
    node -> function_decl.param_capacity = PARAM_INIT_CAP;

    return node;
}

AstNode* ast_make_struct_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Struct;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> struct_decl.fields = arena_alloc(albedo_ctx.arena, sizeof(AstNode) * FIELD_INIT_CAP);
    node -> struct_decl.field_count = 0;
    node -> struct_decl.field_capacity = FIELD_INIT_CAP;

    return node;
}

AstNode* ast_make_enum_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Enum;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> enum_decl.variants = arena_alloc(albedo_ctx.arena, sizeof(AstNode) * VARIANT_INIT_CAP);
    node -> enum_decl.variant_count = 0;
    node -> enum_decl.variant_capacity = VARIANT_INIT_CAP;

    return node;
}

AstNode* ast_make_union_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Union;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> union_decl.fields = arena_alloc(albedo_ctx.arena, sizeof(AstNode) * FIELD_INIT_CAP);
    node -> union_decl.field_count = 0;
    node -> union_decl.field_capacity = FIELD_INIT_CAP;

    return node;
}

AstNode* ast_make_call_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Call;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> call.args = arena_alloc(albedo_ctx.arena, sizeof(AstNode) * ARG_INIT_CAP);
    node -> call.arg_count = 0;
    node -> call.arg_capacity = ARG_INIT_CAP;

    return node;
}

AstNode* ast_make_struct_init_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_StructInit;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> struct_init.fields = arena_alloc(albedo_ctx.arena, sizeof(AstFieldInit) * FIELD_INIT_CAP);
    node -> struct_init.field_count = 0;
    node -> struct_init.field_capacity = FIELD_INIT_CAP;

    return node;
}

AstNode* ast_make_if_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_If;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> if_stmt.branches = arena_alloc(albedo_ctx.arena, sizeof(AstIfBranch) * BRANCH_INIT_CAP);
    node -> if_stmt.branch_count = 0;
    node -> if_stmt.branch_capacity = BRANCH_INIT_CAP;

    return node;
}

AstNode* ast_make_block_node(u32 file_index) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Block;
    node -> flags = 0;

    node -> file_index = file_index;
    node -> span = (Span) {
        .start = 0,
        .end = 0
    };

    node -> resolved_symbol = null;
    node -> resolved_type = null;

    node -> block.stmts = arena_alloc(albedo_ctx.arena, sizeof(AstNode*) * BLOCK_INIT_CAP);
    node -> block.stmt_count = 0;
    node -> block.stmt_capacity = BLOCK_INIT_CAP;

    return node;
}

/* 
*  -------------------------------------
*  | 2. Dynamic array helper functions |
*  -------------------------------------
*/ 
void ast_enum_push_variant(AstNode* node, AstNode variant) {
    if (node -> enum_decl.variant_count >= node -> enum_decl.variant_capacity) {
        usize size = node -> enum_decl.variant_capacity * sizeof(AstNode);

        node -> enum_decl.variants = arena_realloc(
            albedo_ctx.arena,
            node -> enum_decl.variants,
            size,
            size * 2
        );

        node -> enum_decl.variant_capacity *= 2;
    }

    node -> enum_decl.variants[node -> enum_decl.variant_count++] = variant;
}

void ast_struct_push_field(AstNode* node, AstNode field) {
    if (node -> struct_decl.field_count >= node -> struct_decl.field_capacity) {
        usize size = node -> struct_decl.field_capacity * sizeof(AstNode);

        node -> struct_decl.fields = arena_realloc(
            albedo_ctx.arena,
            node -> struct_decl.fields,
            size,
            size * 2
        );

        node -> struct_decl.field_capacity *= 2;
    }

    node -> struct_decl.fields[node -> struct_decl.field_count++] = field;
}

void ast_function_push_param(AstNode* node, AstNode param) {
    if (node -> function_decl.param_count >= node -> function_decl.param_capacity) {
        usize size = node -> function_decl.param_capacity * sizeof(AstNode);

        node -> function_decl.params = arena_realloc(
            albedo_ctx.arena,
            node -> function_decl.params,
            size,
            size * 2
        );

        node -> function_decl.param_capacity *= 2;
    }

    node -> function_decl.params[node -> function_decl.param_count++] = param;
}

void ast_block_push_stmt(AstNode* node, AstNode* stmt) {
    if (node -> block.stmt_count >= node -> block.stmt_capacity) {
        usize size = node -> block.stmt_capacity * sizeof(AstNode*);

        node -> block.stmts = arena_realloc(
            albedo_ctx.arena,
            node -> block.stmts,
            size,
            size * 2
        );

        node -> block.stmt_capacity *= 2;
    }

    node -> block.stmts[node -> block.stmt_count++] = stmt;
}

/* 
*  --------------------------------------
*  | 3. Node attribute helper functions |
*  --------------------------------------
*/ 
inline bool is_resolved(AstNode* node) {
    return node -> flags & IS_RESOLVED;
}

inline bool is_typed(AstNode* node) {
    return node -> flags & IS_TYPED;
}

inline bool is_lvalue(AstNode* node) {
    return node -> flags & IS_LVALUE;
}

inline bool is_const(AstNode* node) {
    return node -> flags & IS_CONST;
}

inline bool is_comp_time(AstNode* node) {
    return node -> flags & IS_COMP_TIME;
}

inline bool is_inline(AstNode* node) {
    return node -> flags & IS_INLINE;
}

inline bool is_external(AstNode* node) {
    return node -> flags & IS_EXTERNAL;
}

inline bool is_static(AstNode* node) {
    return node -> flags & IS_STATIC;
}

inline bool is_variadac(AstNode* node) {
    return node -> flags & IS_VARIADAC;
}
