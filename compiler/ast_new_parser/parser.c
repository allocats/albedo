#include "parser.h"

#include "../albedo/types.h"
#include "../diagnostics/diagnostics.h"

#ifdef DEBUG_MODE
#include <assert.h>
#define assert assert
#else
#define assert(x)
#endif /* ifdef DEBUG_MODE */

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

void extend_ast(void);

void parse_tokens() {
    Parser p = {
        .file_index = 0,
        .cursor = 0,
        .count = albedo_ctx.tokens.count
    };

    while (p.cursor < p.count) {
        extend_ast();

        AstNode* node = null;

        Token* token = parser_peek(&p);

        switch (token -> kind) {
            case T_Import: {
                parser_advance(&p);
                node = parse_import_declaration(&p);
            } break;

            case T_Enum: {
                parser_advance(&p);
                node = parse_enum_declaration(&p);
            } break;

            case T_Struct: {
                parser_advance(&p);
                node = parse_struct_declaration(&p);
            } break;

            case T_Inline: {
                parser_advance(&p);
                
                if (!parser_check(&p, T_Fn)) {
                    err_ast_add(
                        "expected 'fn' after 'inline'",
                        "inline attribute can only be applied to functions",
                        parser_peek(&p),
                        LOC_WHOLE_LINE,
                        p.file_index
                    );

                    break;
                }

                parser_advance(&p);
                node = parse_function_declaration(&p, IS_INLINE);
            } break;

            case T_Extern: {
                parser_advance(&p);

                Token* type = parser_peek(&p);
                
                if (type -> kind == T_Fn) {
                    parser_advance(&p);
                    node = parse_function_declaration(&p, IS_EXTERNAL);
                } else if (type -> kind == T_Let) {
                    // TODO
                    parser_advance(&p);
                    // node = parse_variable_declaration(&p, IS_EXTERNAL);
            } break;

            case T_Eof: {
                p.file_index += 1;
                parser_advance(&p);
            } break;

            default: {
                err_ast_add(
                    "invalid token",
                    "expected top level declaration",
                    token,
                    LOC_WHOLE_TOK,
                    p.file_index
                );
            } break;
        }

        if (node != null) {
            albedo_ctx.ast.nodes[albedo_ctx.ast.count++] = node;
        }
    }
}

void extend_ast(void) {
    if (albedo_ctx.ast.count < albedo_ctx.ast.capacity) return;

    usize size = albedo_ctx.ast.capacity * sizeof(AstNode*);

    albedo_ctx.ast.nodes = arena_realloc(albedo_ctx.arena, albedo_ctx.ast.nodes, size, size * 2);
    albedo_ctx.ast.capacity *= 2;
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
