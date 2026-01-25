#include "../parser.h" 

#include "../../ast/ast.h" 
#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 
#include "../../token/token.h" 

#include <stdio.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* parse_block(Parser* p) {
    AstNode* node = ast_make_block_node();

    u32 depth = 1;

    parser_advance(p);

    while (depth > 0 && p -> cursor < p -> count) {
        Token* token = parser_peek(p);

        if (token -> kind == T_LBrace) {
            depth++;

            parser_advance(p);

            continue;
        } else if (token -> kind == T_RBrace) {
            depth--;

            parser_advance(p);

            if (depth == 0) {
                return node;
            };

            continue;
        }

        switch (token -> kind) {
            case T_Let: {
                parser_advance(p);

                AstNode* stmt = parse_var_decl(p);

                if (!parser_check(p, T_Semi)) {
                    err_ast_add(
                        "expected ';'",
                        "add a ';' here",
                        parser_peek_prev(p),
                        LOC_END_OF_TOK,
                        p -> file_index
                    );

                    // recover
                } 

                parser_advance(p);

                ast_block_stmt_push(node, stmt);
            } break;

            default: {
                // error
            } break;
        }
    }

    // Can only reach this if there is depth issue
    return top_level_decl_parse_fail(p, node);
}

AstNode* parse_var_decl(Parser* p) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_VarDecl;

    Token* name = parser_peek(p);

    if (name -> kind != T_Ident) {
        err_ast_add(
            "expected variable name",
            "add a valid variable name here",
            name,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        // todo: return fail 
        return null;
    }

    node -> var_decl.name_ptr = name -> lexeme;
    node -> var_decl.name_len = name -> length;
    node -> var_decl.value = null;

    parser_advance(p);

    if (!parser_check(p, T_Colon)) {
        err_ast_add(
            "expected ':' to define type",
            "add a ':' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        // todo: return fail 
        return null;
    }

    parser_advance(p);

    AstSpan type_span = parse_type_span(p);

    if (type_span.start_index == 0 && type_span.end_index == 0) {
        err_ast_add(
            "expected a valid type",
            "add a valid type here",
            parser_peek(p),
            LOC_WHOLE_LINE,
            p -> file_index
        );

        // todo: return fail 
        return null;
    }

    node -> var_decl.type = type_span;

    if (parser_check(p, T_Semi)) {
        return node;
    }

    if (parser_check(p, T_Eq)) {
        parser_advance(p);

        AstNode* value = parse_expression(p);

        node -> var_decl.value = value;
    }

    return node;
}
