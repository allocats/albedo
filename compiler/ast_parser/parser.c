#include "parser.h"

#include "../albedo/types.h"
#include "../diagnostics/types.h"

#include <assert.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

void parse_tokens(void) {
    Parser parser = {
        .file_index = 0,
        .cursor = 0,
        .count = albedo_ctx.tokens.count
    };

    while (parser.cursor < parser.count) {
        extend_parse_tree(albedo_ctx.arena, &albedo_ctx.ast);

        Token* token = parser_advance(&parser);
        AstNode* node = null;

        switch (token -> kind) {
            case T_Module: {
                node = parse_module_decl(&parser);
            } break;

            case T_Import: {
                node = parse_import_decl(&parser);
            } break;

            case T_Ident: {
                // emit error: not a top level decl 
                // levenshtein distance for suggestion
            } break;

            case T_Eof: {
                parser.file_index++;
            } break;

            default: {
            } break;
        }

        if (node) {
            albedo_ctx.ast.nodes[albedo_ctx.ast.count++] = node;
        }
    }
}

void extend_parse_tree(ArenaAllocator* arena, ParseTree* tree) {
    if (tree -> count >= tree -> capacity) {
        usize size = sizeof(AstNode*) * tree -> capacity;

        tree -> nodes = arena_realloc(arena, tree -> nodes, size, size * 2);
        tree -> capacity *= 2;
    }
}

inline Token* parser_peek(Parser* p) {
    assert(p -> cursor < p -> count && "Array out of bounds");
    return &albedo_ctx.tokens.items[p -> cursor];
}

inline Token* parser_peek_prev(Parser* p) {
    assert(p -> cursor > 0 && p -> cursor <= p -> count && "Array out of bounds");
    return &albedo_ctx.tokens.items[p -> cursor - 1];
}

inline Token* parser_advance(Parser* p) {
    assert(p -> cursor < p -> count && "Array out of bounds");
    return &albedo_ctx.tokens.items[p -> cursor++];
}

inline bool parser_check(Parser* p, TokenKind kind) {
    return parser_peek(p) -> kind == kind;
}
