#include "ast.h"

#include "../albedo/types.h"
#include "../diagnostics/types.h"
#include "types.h"

#include <assert.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* ast_make_fn_node(bool is_extern) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Function;
    node -> function_decl.is_extern = is_extern;
    node -> function_decl.generics = arena_alloc(
            arena,
            sizeof(AstGenericParam) * PARAM_DEFAULT_CAP
            );
    node -> function_decl.generic_count = 0;
    node -> function_decl.generic_capacity = PARAM_DEFAULT_CAP;

    node -> function_decl.params = arena_alloc(
            arena,
            sizeof(AstParam) * PARAM_DEFAULT_CAP
            );
    node -> function_decl.param_count = 0;
    node -> function_decl.param_capacity = PARAM_DEFAULT_CAP;

    node -> function_decl.return_type = (AstSpan) {0};

    return node;
}

AstNode* ast_make_struct_node(bool is_extern) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Struct;
    node -> struct_decl.is_extern = is_extern;
    node -> struct_decl.generics = arena_alloc(
            arena,
            sizeof(AstGenericParam) * FIELD_DEFAULT_CAP 
            );
    node -> struct_decl.generic_count = 0;
    node -> struct_decl.generic_capacity = FIELD_DEFAULT_CAP;

    node -> struct_decl.fields = arena_alloc(
            arena,
            sizeof(AstField) * FIELD_DEFAULT_CAP
            );
    node -> struct_decl.field_count = 0;
    node -> struct_decl.field_capacity = FIELD_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_enum_node(bool is_extern) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Enum;
    node -> enum_decl.is_extern = is_extern;
    node -> enum_decl.variants = arena_alloc(
        arena,
        sizeof(AstEnumVariant) * ENUM_DEFAULT_CAP
    );
    node -> enum_decl.variant_count = 0;
    node -> enum_decl.variant_capacity = ENUM_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_union_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Union;
    node -> union_decl.fields = arena_alloc(
        arena, 
        sizeof(AstField) * UNION_DEFAULT_CAP
    );
    node -> union_decl.field_count = 0;
    node -> union_decl.field_count = UNION_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_tunion_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Tunion;
    node -> tunion_decl.variants = arena_alloc(
        arena, 
        sizeof(AstTunionVariant) * TUNION_DEFAULT_CAP
    );
    node -> tunion_decl.variant_count = 0;
    node -> tunion_decl.variant_count = TUNION_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_if_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_If;
    node -> if_stmt.branches = arena_alloc(arena, sizeof(AstIfBranch) * MATCH_DEFAULT_CAP);
    node -> if_stmt.branch_count = 0; 
    node -> if_stmt.branch_capacity = MATCH_DEFAULT_CAP; 

    return node;
}

AstNode* ast_make_match_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Match;
    node -> match_stmt.arms = arena_alloc(
        arena,
        sizeof(AstMatchArm) * MATCH_DEFAULT_CAP
    );
    node -> match_stmt.arm_count = 0;
    node -> match_stmt.arm_capacity = MATCH_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_block_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Block;
    node -> block.stmts = arena_alloc(
        arena,
        sizeof(AstNode*) * BLOCK_DEFAULT_CAP
    );
    node -> block.stmt_count = 0;
    node -> block.stmt_capacity = BLOCK_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_return_node(AstNode* expr) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Return;
    node -> return_stmt.expr = expr;

    return node;
}

AstNode* ast_make_defer_node(AstNode* stmt) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Defer;
    node -> defer_stmt.stmt = stmt;

    return node;
}

AstNode* ast_make_fn_call_node(AstNode* ident) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Call;
    node -> fn_call.ident = ident;
    node -> fn_call.args = arena_alloc(
        arena,
        sizeof(AstNode*) * PARAM_DEFAULT_CAP
    );
    node -> fn_call.arg_count = 0;
    node -> fn_call.arg_capacity = PARAM_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_struct_init_node(AstNode* identifier) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_StructInit;
    node -> struct_init.ident = identifier;
    node -> struct_init.field_inits = arena_alloc(
        arena,
        sizeof(AstFieldInit) * FIELD_DEFAULT_CAP
    );
    node -> struct_init.field_count = 0;
    node -> struct_init.field_capacity = FIELD_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_loop_node(AstNode* block) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Loop;

    node -> loop_loop.block = block; 

    return node;
}

AstNode* ast_make_while_node(AstNode* condition, AstNode* block) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_While;

    node -> while_loop.condition = condition; 
    node -> while_loop.block = block; 

    return node;
}

AstNode* ast_make_for_node(AstNode* iter, AstNode* condition, AstNode* step, AstNode* block) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_For;

    node -> for_loop.iterator = iter; 
    node -> for_loop.condition = condition; 
    node -> for_loop.step = step; 
    node -> for_loop.block = block; 

    return node;
}

AstNode* ast_make_index_node(AstNode* ident, AstNode* index) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Index;

    node -> index_access.ident = ident;
    node -> index_access.index = index;

    return node;
}

AstNode* ast_make_member_access_node(AstNode* ident, Token* field) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_MemberAccess;

    node -> member_access.target = ident;
    node -> member_access.field_ptr = field -> lexeme;
    node -> member_access.field_len = field -> length;

    return node;
}

AstNode* ast_make_literal_node(Token* token) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Literal;

    node -> literal.kind = token -> kind;
    node -> literal.ptr = token -> lexeme;
    node -> literal.len = token -> length;

    return node;
}

AstNode* ast_make_ident_node(Token* token) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Ident;

    node -> ident.namespaces = arena_alloc(arena, sizeof(AstSegment) * IMPORT_DEFAULT_CAP); 
    node -> ident.namespace_count = 0;
    node -> ident.namespace_capacity = IMPORT_DEFAULT_CAP;

    if (token) {
        node -> ident.ptr = token -> lexeme;
        node -> ident.len = token -> length;
    }

    return node;
}

AstNode* ast_make_assign_node(AstNode* target, AstNode* value, TokenKind op) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Assign;

    node -> assign.target = target;
    node -> assign.value = value;
    node -> assign.op = op;

    return node;
}

AstNode* ast_make_binop_node(AstNode* left, AstNode* right, TokenKind op) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Binary;

    node -> binary_op.left = left;
    node -> binary_op.op = op;
    node -> binary_op.right = right;

    return node;
}

AstNode* ast_make_cast_node(AstNode* expr, AstSpan type) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Cast;

    node -> cast.expr = expr;
    node -> cast.target_type = type;

    return node;
}

AstNode* ast_make_unary_node(AstNode* operand, TokenKind op) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Unary;

    node -> unary_op.operand = operand;
    node -> unary_op.op = op;

    return node;
}

// 
//
//
//

void ast_init_module_decl(AstNode* node) {
    assert(node);

    ArenaAllocator* arena = albedo_ctx.arena;

    node -> module_decl.segments = arena_alloc(
        arena,
        sizeof(AstSegment) * IMPORT_DEFAULT_CAP
    );
    node -> module_decl.segment_count = 0;
    node -> module_decl.segment_capacity = IMPORT_DEFAULT_CAP;
}

void ast_module_segment_push(AstNode* node, Token* token) {
    u32* count = &node -> module_decl.segment_count;
    u32* capacity = &node -> module_decl.segment_capacity;

    if (*count >= *capacity) {
        usize size = *capacity * sizeof(AstSegment);

        node -> module_decl.segments = arena_realloc(
            albedo_ctx.arena,
            node -> module_decl.segments,
            size,
            size * 2
        );

        *capacity *= 2;
    }

    node -> module_decl.segments[(*count)++] = (AstSegment) {
        .ptr = token -> lexeme,
        .len = token -> length
    };
}
void ast_init_lib_import(AstNode* node) {
    assert(node);

    ArenaAllocator* arena = albedo_ctx.arena;

    node -> import_decl.lib.segments = arena_alloc(
        arena,
        sizeof(AstSegment) * IMPORT_DEFAULT_CAP
    );
    node -> import_decl.lib.segment_count = 0;
    node -> import_decl.lib.segment_capacity = IMPORT_DEFAULT_CAP;
}

void ast_import_lib_push(AstNode* node, Token* token) {
    u32* count = &node -> import_decl.lib.segment_count;
    u32* capacity = &node -> import_decl.lib.segment_capacity;

    if (*count >= *capacity) {
        usize size = *capacity * sizeof(AstSegment);

        node -> import_decl.lib.segments = arena_realloc(
            albedo_ctx.arena,
            node -> import_decl.lib.segments,
            size,
            size * 2
        );

        *capacity *= 2;
    }

    node -> import_decl.lib.segments[(*count)++] = (AstSegment) {
        .ptr = token -> lexeme,
        .len = token -> length
    };
}

void ast_fn_param_push(AstNode* node, AstParam param) {
    if (node -> function_decl.param_count >= node -> function_decl.param_capacity) {
        usize size = node -> function_decl.param_capacity * sizeof(AstParam);

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

void ast_fn_call_arg_push(AstNode* node, AstNode* arg) {
    if (node -> fn_call.arg_count >= node -> fn_call.arg_capacity) {
        usize size = node -> fn_call.arg_capacity * sizeof(AstNode*);

        node -> fn_call.args = arena_realloc(
            albedo_ctx.arena,
            node -> fn_call.args,
            size,
            size * 2
        );
        node -> fn_call.arg_capacity *= 2;
    }

    node -> fn_call.args[node -> fn_call.arg_count++] = arg;
}

void ast_fn_generic_push(AstNode* node, Token* token) {
    if (node -> function_decl.generic_count >= node -> function_decl.generic_capacity) {
        usize size = node -> function_decl.generic_capacity * sizeof(AstGenericParam);

        node -> function_decl.generics = arena_realloc(
            albedo_ctx.arena,
            node -> function_decl.generics,
            size,
            size * 2
        );
        node -> function_decl.generic_capacity *= 2;
    }

    node -> function_decl.generics[node -> function_decl.generic_count++] = (AstGenericParam) {
        .name_ptr = token -> lexeme,
        .name_len = token -> length
    };
}

void ast_struct_generic_push(AstNode* node, Token* token) {
    if (node -> struct_decl.generic_count >= node -> struct_decl.generic_capacity) {
        usize size = node -> struct_decl.generic_capacity * sizeof(AstGenericParam);

        node -> struct_decl.generics = arena_realloc(
            albedo_ctx.arena,
            node -> struct_decl.generics,
            size,
            size * 2
        );
        node -> struct_decl.generic_capacity *= 2;
    }

    node -> struct_decl.generics[node -> struct_decl.generic_count++] = (AstGenericParam) {
        .name_ptr = token -> lexeme,
        .name_len = token -> length
    };
}

void ast_struct_field_push(AstNode* node, AstField field) {
    if (node -> struct_decl.field_count >= node -> struct_decl.field_capacity) {
        usize size = node -> struct_decl.field_capacity * sizeof(AstField);

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

void ast_struct_init_field_push(AstNode* node, AstFieldInit field) {
    if (node -> struct_init.field_count >= node -> struct_init.field_capacity) {
        usize size = node -> struct_init.field_capacity * sizeof(AstFieldInit);

        node -> struct_init.field_inits = arena_realloc(
            albedo_ctx.arena,
            node -> struct_init.field_inits,
            size,
            size * 2
        );
        node -> struct_init.field_capacity *= 2;
    }

    node -> struct_init.field_inits[node -> struct_init.field_count++] = field;
}

void ast_enum_variant_push(AstNode* node, AstEnumVariant variant) {
    if (node -> enum_decl.variant_count >= node -> enum_decl.variant_capacity) {
        usize size = node -> enum_decl.variant_capacity * sizeof(AstEnumVariant);

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


void ast_block_stmt_push(AstNode* node, AstNode* stmt) {
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

void ast_if_branch_push(AstNode* node, AstNode* condition, AstNode* block) {
    if (node -> if_stmt.branch_count >= node -> if_stmt.branch_capacity) {
        usize size = node -> if_stmt.branch_capacity * sizeof(AstIfBranch);

        node -> if_stmt.branches = arena_realloc(
            albedo_ctx.arena,
            node -> if_stmt.branches,
            size,
            size * 2
        );

        node -> if_stmt.branch_capacity *= 2;
    }

    node -> if_stmt.branches[node -> if_stmt.branch_count++] = (AstIfBranch) {
        .condition = condition,
        .block = block
    };
}

void ast_ident_namespace_push(AstNode* node, Token* token) {
    if (node -> ident.namespace_count >= node -> ident.namespace_capacity) {
        usize size = node -> ident.namespace_capacity * sizeof(AstSegment);

        node -> ident.namespaces = arena_realloc(
            albedo_ctx.arena,
            node -> ident.namespaces,
            size,
            size * 2
        );

        node -> ident.namespace_capacity *= 2;
    }

    node -> ident.namespaces[node -> ident.namespace_count++] = (AstSegment) {
        .len = token -> length,
        .ptr = token -> lexeme
    };

    node -> ident.len = token -> length;
    node -> ident.ptr = token -> lexeme;
}
