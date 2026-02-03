#include "../parser.h" 

#include "../../ast/ast.h" 
#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

#define CHECK_SEMI                  \
    if (!parser_check(p, T_Semi)) { \
        err_ast_add(                \
            "expected ';'",         \
            "add a ';' here",       \
            parser_peek_prev(p),    \
            LOC_END_OF_TOK,         \
            p -> file_index         \
            );                      \
        recover_stmt(p);            \
        continue;                   \
    } 

#define PARSE_STMT(fn)                      \
    do {                                    \
        parser_advance(p);                  \
        AstNode* stmt = fn(p);              \
        CHECK_SEMI;                         \
        parser_advance(p);                  \
        ast_block_stmt_push(node, stmt);    \
    } while(0);

AstNode* parse_block(Parser* p) {
    AstNode* node = ast_make_block_node();

    parser_advance(p);

    while (p -> cursor < p -> count) {
        Token* token = parser_peek(p);

        switch (token -> kind) {
            case T_LBrace: {
                AstNode* block_stmt = parse_block(p);
                ast_block_stmt_push(node, block_stmt);
            } break;

            case T_RBrace: {
                parser_advance(p);
                return node;
            };

            case T_Let: {
                PARSE_STMT(parse_var_decl);
            } break;

            case T_Const: {
                PARSE_STMT(parse_const_decl);
            } break;

            case T_Return: {
                PARSE_STMT(parse_return);
            } break;

            case T_Defer: {
                PARSE_STMT(parse_defer);
            } break;

            case T_If: {
                AstNode* stmt = parse_if_stmt(p);
                ast_block_stmt_push(node, stmt);
            } break;

            case T_For: {
                parser_advance(p);
                AstNode* stmt = parse_for_loop(p);
                ast_block_stmt_push(node, stmt);
            } break;

            case T_While: {
                parser_advance(p);
                AstNode* stmt = parse_while_loop(p);
                ast_block_stmt_push(node, stmt);
            } break;

            case T_Loop: {
                parser_advance(p);
                AstNode* stmt = parse_loop_stmt(p);
                ast_block_stmt_push(node, stmt);
            } break;

            case T_Ident: {
                AstNode* stmt = parse_expression(p);
                
                if (!stmt) {
                    recover_stmt(p);
                    continue;
                }

                CHECK_SEMI;

                parser_advance(p);

                ast_block_stmt_push(node, stmt);
            } break;

            default: {
                err_ast_add(
                    "expected statement",
                    null,
                    token,
                    LOC_WHOLE_LINE,
                    p -> file_index
                );

                recover_stmt(p);
            } break;
        }
    }

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

        return stmt_parse_fail(p, node);
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

        return stmt_parse_fail(p, node);
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

        return stmt_parse_fail(p, node);
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

AstNode* parse_const_decl(Parser* p) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Const;

    Token* name = parser_peek(p);

    if (name -> kind != T_Ident) {
        err_ast_add(
            "expected constant name",
            "add a valid constant name here",
            name,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return stmt_parse_fail(p, node);
    } 

    node -> const_decl.name_len = name -> length;
    node -> const_decl.name_ptr = name -> lexeme;

    parser_advance(p);

    if (!parser_check(p, T_Colon)) {
        err_ast_add(
            "expected ':' to define type",
            "add a ':' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return stmt_parse_fail(p, node);
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

        return stmt_parse_fail(p, node);
    }

    node -> const_decl.type = type_span;

    if (!parser_check(p, T_Eq)) {
        err_ast_add(                
            "expected '='",         
            "add a '=' here, constants MUST be initialised",       
            parser_peek_prev(p),    
            LOC_END_OF_TOK,         
            p -> file_index         
        );                      

        return stmt_parse_fail(p, node);
    }

    parser_advance(p);

    AstNode* value = parse_expression(p);

    if (!value) {
        err_ast_add(                
            "expected expression",         
            "constants must be initialised",       
            parser_peek_prev(p),    
            LOC_WHOLE_LINE,         
            p -> file_index         
        );                      

        return stmt_parse_fail(p, node);
    }

    node -> const_decl.value = value;

    return node;
}

AstNode* parse_return(Parser* p) {
    AstNode* expr = parse_expression(p);
    return ast_make_return_node(expr);
}

AstNode* parse_defer(Parser* p) {
    AstNode* expr = parse_expression(p);
    return ast_make_defer_node(expr);
}

AstNode* parse_if_stmt(Parser* p) {
    AstNode* node = ast_make_if_node();

    Token* if_kw = parser_advance(p);

    AstNode* condition = parse_expression(p);

    if (!condition) {
        err_ast_add(
            "expected expression",
            "add an expression",
            if_kw,
            LOC_WHOLE_LINE,
            p -> file_index
        );

        recover_control_stmt_condition(p);
    }

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{'",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return control_stmt_parse_fail(p, node);
    }

    AstNode* block = parse_block(p);

    ast_if_branch_push(node, condition, block);

    while (parser_check(p, T_Else)) {
        parser_advance(p);

        if (parser_check(p, T_LBrace)) {
            if (node -> if_stmt.else_block != null) {
                err_ast_add(
                    "else block already defined",
                    "did you mean else if?",
                    parser_peek_prev(p),
                    LOC_WHOLE_TOK,
                    p -> file_index
                );

                return control_stmt_parse_fail(p, node);
            }

            node -> if_stmt.else_block = parse_block(p);

            return node;
        }

        if (!parser_check(p, T_If)) {
            err_ast_add(
                "expected 'if' or '{'",
                "add a 'if' or '{' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return null;
        }
        
        parser_advance(p);

        AstNode* branch_condition = parse_expression(p);
        
        if (!parser_check(p, T_LBrace)) {
            err_ast_add(
                    "expected '{'",
                    "add a '{'",
                    parser_peek_prev(p),
                    LOC_END_OF_TOK,
                    p -> file_index
                    );

            return control_stmt_parse_fail(p, node);
        }
        
        AstNode* branch_block = parse_block(p);

        ast_if_branch_push(node, branch_condition, branch_block);
    }

    return node;
}

AstNode* parse_loop_stmt(Parser* p) {
    AstNode* node = ast_make_loop_node();

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return control_stmt_parse_fail(p, node);
    }

    AstNode* block = parse_block(p);

    node -> loop_loop.block = block;

    return node;
}

AstNode* parse_while_loop(Parser* p) {
    AstNode* node = ast_make_while_node();

    AstNode* condition = parse_expression(p);

    if (!condition) {
        recover_control_stmt_condition(p);
    }

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return control_stmt_parse_fail(p, node);
    }

    AstNode* block = parse_block(p);

    node -> while_loop.condition = condition;
    node -> while_loop.block = block;

    return node;
}

AstNode* parse_for_loop(Parser* p) {
    AstNode* node = ast_make_for_node();

    AstNode* iterator = parse_var_decl(p);

    if (!parser_check(p, T_Semi)) {
        err_ast_add(
            "expected ';'",
            "add a ';' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        // TODO: Add a nice recovery to block
        return control_stmt_parse_fail(p, node);
    }

    parser_advance(p);

    AstNode* condition = parse_expression(p);

    if (!condition) {
        // TODO: Add a nice recovery to block
        return control_stmt_parse_fail(p, node);
    }

    if (!parser_check(p, T_Semi)) {
        err_ast_add(
            "expected ';'",
            "add a ';' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        // TODO: Add a nice recovery to block
        return control_stmt_parse_fail(p, node);
    }

    parser_advance(p);

    AstNode* step = parse_expression(p);

    if (!step) {
        // TODO: Add a nice recovery to block
        return control_stmt_parse_fail(p, node);
    }

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return control_stmt_parse_fail(p, node);
    }

    AstNode* block = parse_block(p);

    node -> for_loop.iterator = iterator;
    node -> for_loop.condition = condition;
    node -> for_loop.step = step;
    node -> for_loop.block = block;

    return node;
}
