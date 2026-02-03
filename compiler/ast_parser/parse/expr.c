#include "../parser.h" 

#include "../../ast/ast.h" 
#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* parse_expr_prec(Parser* p, Precedence min_prec);
AstNode* parse_primary(Parser* p);
AstNode* parse_prefix(Parser* p);
AstNode* parse_postfix(Parser* p, AstNode* node); 

AstNode* parse_struct_init(Parser* p, AstNode* ident);

inline AstNode* parse_expression(Parser* p) {
    return parse_expr_prec(p, PREC_ASSIGNMENT);
}

AstNode* parse_expr_prec(Parser* p, Precedence min_prec) {
    AstNode* left = parse_prefix(p);

    while (p -> cursor < p -> count) {
        Token* token = parser_peek(p);
        TokenKind op = token -> kind;

        switch (op) {
            case T_LParen:
            case T_LBracket:
            case T_Dot:
            case T_Arrow: {
                left = parse_postfix(p, left);
                continue;
            }

            default: break;
        }

        Precedence prec = get_precedence(op); 
        if (prec < min_prec) {
            break;
        }

        if (op == T_As) {
            parser_advance(p);

            AstSpan type_span = parse_type_span(p);

            if (type_span.start_index == 0) {
                return null;
            }

            left = ast_make_cast_node(left, type_span);

            continue;
        }

        Precedence next_min = is_right_associative(prec) ? prec : (prec + 1);

        parser_advance(p);

        AstNode* right = parse_expr_prec(p, next_min);

        if (prec == PREC_ASSIGNMENT) {
            left = ast_make_assign_node(left, right, op);
        } else {
            left = ast_make_binop_node(left, right, op);
        }
    }

    return left;
}

AstNode* parse_primary(Parser* p) {
    Token* token = parser_peek(p);

    switch (token -> kind) {
        case T_Null:
        case T_IntLit:
        case T_FloatLit:
        case T_CharLit:
        case T_StrLit:
        case T_True:
        case T_False: {
            parser_advance(p);
            return ast_make_literal_node(token);
        }

        case T_Ident: {
            AstNode* node = parse_qualified_name(p);
            
            if (parser_check(p, T_LBrace)) {
                parser_advance(p);
                return parse_struct_init(p, node);
            }

            return node;
        }

        case T_LParen: {
            parser_advance(p);

            AstNode* expression = parse_expression(p);

            if (!parser_check(p, T_RParen)) {
                err_ast_add(
                    "expected ')' after expression",
                    "add a ')' here",
                    parser_peek_prev(p),
                    LOC_END_OF_TOK,
                    p -> file_index
                );
            } else {
                parser_advance(p);
            }

            return expression;
        }

        default: {
            err_ast_add(
                "expected expression",
                "add an expression here",
                token,
                LOC_WHOLE_LINE,
                p -> file_index
            );

            return null;
        }
    }
}

AstNode* parse_prefix(Parser* p) {
    Token* token = parser_peek(p);

    if (is_unary_operator(token -> kind)) {
        parser_advance(p);

        AstNode* operand = parse_expr_prec(p, PREC_UNARY);

        return ast_make_unary_node(operand, token -> kind);
    }

    return parse_primary(p);
}

AstNode* parse_postfix(Parser* p, AstNode* node) {
    while (p -> cursor < p -> count) {
        Token* token = parser_peek(p);

        switch (token -> kind) {
            case T_LParen: {
                parser_advance(p);

                AstNode* call = ast_make_fn_call_node(node);

                while (p -> cursor < p -> count && !parser_check(p, T_RParen)) {
                    AstNode* arg = parse_expression(p);

                    ast_fn_call_arg_push(call, arg);

                    if (parser_check(p, T_Comma)) {
                        parser_advance(p);

                        if (parser_check(p, T_RParen)) {
                            break;
                        }
                    } else if (!parser_check(p, T_RParen)) {
                        err_ast_add(
                            "expected ',' or ')' in function call",
                            "add a ',' or ')' here",
                            parser_peek_prev(p),
                            LOC_END_OF_TOK,
                            p -> file_index
                        );

                        // fail
                    }
                }

                if (!parser_check(p, T_RParen)) {
                    err_ast_add(
                        "expected ')' in function call",
                        "add a ')' here",
                        parser_peek_prev(p),
                        LOC_END_OF_TOK,
                        p -> file_index
                    );
                    
                    // fail
                } else {
                    parser_advance(p);
                }

                node = call;
            } break;

            case T_LBracket: {
                parser_advance(p);

                AstNode* index_expr = parse_expression(p); 

                if (!parser_check(p, T_RBracket)) {
                    err_ast_add(
                        "expected ']'",
                        "add a ']' here",
                        parser_peek_prev(p),
                        LOC_END_OF_TOK,
                        p -> file_index
                    );

                    return null;
                }

                parser_advance(p);

                node = ast_make_index_node(node, index_expr);
            } break;

            case T_Dot: {
                parser_advance(p);

                Token* field = parser_peek(p);

                if (field -> kind != T_Ident) {
                    err_ast_add(
                        "expected field name after '.'",
                        "add a valid field name here",
                        field,
                        LOC_WHOLE_TOK,
                        p -> file_index
                    );

                    return null;
                }

                parser_advance(p);

                node = ast_make_member_access_node(node, field);
            } break;

            case T_Arrow: {
                parser_advance(p);

                Token* field = parser_peek(p);

                if (field -> kind != T_Ident) {
                    err_ast_add(
                        "expected field name after '->'",
                        "add a valid field name here",
                        field,
                        LOC_WHOLE_TOK,
                        p -> file_index
                    );

                    return null;
                }

                parser_advance(p);

                AstNode* deref = ast_make_unary_node(node, T_Star);
                AstNode* access = ast_make_member_access_node(deref, field);

                node = access;
            } break;

            default: {
                return node;
            } break;
        }
    }
    
    return null;
}

AstNode* parse_struct_init(Parser* p, AstNode* ident) {
    AstNode* node = ast_make_struct_init_node(ident);

    node -> span.start = p -> cursor - 2;

    while (!parser_check(p, T_RBrace)) {
        if (!parser_check(p, T_Dot)) {
            err_ast_add(
                "expected '.'",
                "add a '.' before a field",
                parser_peek(p),
                LOC_START_OF_TOK,
                p -> file_index
            );

            recover_struct_init(p);
            continue;
        }

        parser_advance(p);

        Token* field_name = parser_peek(p);

        if (field_name -> kind != T_Ident) {
            err_ast_add(
                "expected identifier",
                "add a valid identifier",
                field_name,
                LOC_WHOLE_TOK,
                p -> file_index
            );

            recover_struct_init(p);
            continue;
        }

        parser_advance(p);

        if (!parser_check(p, T_Eq)) {
            err_ast_add(
                "expected '='",
                "add a '=' after a field",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            recover_struct_init(p);
            continue;
        } 

        parser_advance(p);

        AstNode* field_value = parse_expression(p);

        if (!field_value) {
            recover_struct_init(p);
            continue;
        }

        AstFieldInit field = {
            .len = field_name -> length,
            .ptr = field_name -> lexeme,
            .value = field_value 
        };

        ast_struct_init_field_push(node, field);
        
        if (parser_check(p, T_Comma)) {
            parser_advance(p);

            if (parser_check(p, T_RBrace)) {
                break;
            }
        } else if (!parser_check(p, T_RBrace)) {
            err_ast_add(
                "expected ',' or '}'",
                "add a ',' between fields or '}' to end the initializer",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            recover_struct_init(p);
            continue;
        }
    }

    if (!parser_check(p, T_RBrace)) {
        err_ast_add(
            "expected '}'",
            "add a '}' to close the struct initializer",
            parser_peek(p),
            LOC_START_OF_TOK,
            p -> file_index
        );
        
        recover_struct_init(p);

        if (parser_check(p, T_Semi)) {
            parser_advance(p);
            return node;
        }
    }

    parser_advance(p);

    node -> span.end = p -> cursor;

    return node;
}
