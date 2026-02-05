#include "parser.h"

#include "../albedo/types.h"
#include "../diagnostics/diagnostics.h"

#include <assert.h>
#include <stdio.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

void parse_tokens(usize starting_token_index, usize starting_file_index) {
    Parser parser = {
        .file_index = starting_file_index,
        .cursor = starting_token_index,
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

            case T_Extern: {
                Token* next = parser_advance(&parser);

                switch (next -> kind) {
                    case T_Fn: {
                        node = parse_fn_decl(&parser, true);
                    } break;

                    case T_Struct: {
                        node = parse_struct_decl(&parser, true);
                    } break;

                    case T_Enum: {
                        node = parse_enum_decl(&parser, true);
                    } break;

                    default: {
                        // TODO: Error
                        printf("Found: %s\n", TOKEN_KIND_STRINGS[next -> kind]);
                        assert(0 > 1);
                    } break;
                }
            } break;


            case T_Const: {
                node = parse_const_decl(&parser);
                
                if (!parser_check(&parser, T_Semi)) {
                    err_ast_add(
                        "expected ';'",
                        "add a ';' here",
                        parser_peek_prev(&parser),
                        LOC_END_OF_TOK,
                        parser.file_index
                    );

                    node = null;
                } else {
                    parser_advance(&parser);
                }
            } break;

            case T_Struct: {
                node = parse_struct_decl(&parser, false);
            } break;

            case T_Enum: {
                node = parse_enum_decl(&parser, false);
            } break;
                           
            case T_Fn: {
                node = parse_fn_decl(&parser, false);
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
            node -> file_index = parser.file_index;
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
