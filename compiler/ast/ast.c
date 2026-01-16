#include "ast.h"

#include "../albedo/types.h"
#include "../diagnostics/types.h"
#include "types.h"

#include <assert.h>
#include <stdint.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* ast_make_fn_node(bool is_extern) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    if (is_extern) {
        node -> kind = A_Function;
        node -> extern_fn.params = arena_alloc(
            arena,
            sizeof(AstParam) * PARAM_DEFAULT_CAP
        );
        node -> extern_fn.param_count = 0;
        node -> extern_fn.param_capacity = PARAM_DEFAULT_CAP;
    } else {
        node -> kind = A_Function;

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
    }

    return node;
}

AstNode* ast_make_struct_node(bool is_extern) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    if (is_extern) {
        node -> kind = A_ExternStruct;
        node -> extern_struct.fields = arena_alloc(
            arena,
            sizeof(AstField) * FIELD_DEFAULT_CAP
        );
        node -> extern_struct.field_count = 0;
        node -> extern_struct.field_capacity = FIELD_DEFAULT_CAP;
    } else {
        node -> kind = A_Struct;

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
    }

    return node;
}

AstNode* ast_make_enum_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Enum;
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

AstNode* ast_make_fn_call_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_Call;
    node -> fn_call.args = arena_alloc(
        arena,
        sizeof(AstNode*) * PARAM_DEFAULT_CAP
    );
    node -> fn_call.arg_count = 0;
    node -> fn_call.arg_capacity = PARAM_DEFAULT_CAP;

    return node;
}

AstNode* ast_make_struct_init_node(void) {
    ArenaAllocator* arena = albedo_ctx.arena;

    AstNode* node = arena_alloc(arena, sizeof(*node));

    node -> kind = A_StructInit;
    node -> struct_init.field_inits = arena_alloc(
        arena,
        sizeof(AstNode*) * FIELD_DEFAULT_CAP
    );
    node -> struct_init.field_count = 0;
    node -> struct_init.field_capacity = FIELD_DEFAULT_CAP;

    return node;
}

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

    node -> module_decl.segments[*count++] = (AstSegment) {
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

    node -> import_decl.lib.segments[*count++] = (AstSegment) {
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
