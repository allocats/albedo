#include "../parser.h"

#include "../../ast_new/ast.h"
#include "../../diagnostics/diagnostics.h"

bool parse_parameters(Parser* p, AstNode* node);

AstNode* parse_import_declaration(Parser* p) {
    AstNode* node = ast_make_generic_blank_node(A_Import, 0, p -> file_index); 

    Token* token = parser_peek(p);

    if (token -> kind != T_StrLit) {
        err_ast_add(
            "expected string literal",
            "add a valid string literal here",
            token,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    parser_advance(p);

    node -> import_decl.len = token -> length - 2;
    node -> import_decl.ptr = token -> lexeme + 1;

    if (!parser_check(p, T_Semi)) {
        err_ast_add(
            "expected ';'",
            "add a ';' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return node;
    }

    parser_advance(p);

    return node;
}

AstNode* parse_enum_declaration(Parser* p) {
    AstNode* node = ast_make_struct_node(p -> file_index);

    Token* enum_name = parser_peek(p);

    if (enum_name -> kind != T_Ident) {
        err_ast_add(
            "expected identifier for enum name",
            "add a valid identifier here",
            enum_name,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> enum_decl.ptr = enum_name -> lexeme;
    node -> enum_decl.len = enum_name -> length;

    parser_advance(p);

    if (parser_check(p, T_Colon)) {
        parser_advance(p);

        // Just going to support integers for now
        if (!parser_check(p, T_Ident)) {
            err_ast_add(
                "expected '{'",
                "add a '{' here",
                enum_name,
                LOC_END_OF_TOK,
                p -> file_index
            );

            // recover
        }



        // TODO: Parse type
    }

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );
    }

    parser_advance(p);

    while (!parser_check(p, T_RBrace)) {
        Token* field_name = parser_peek(p);

    }
}

AstNode* parse_struct_declaration(Parser* p) {
    AstNode* node = ast_make_struct_node(p -> file_index);

    Token* struct_name = parser_peek(p);

    if (struct_name -> kind != T_Ident) {
        err_ast_add(
            "expected identifier for struct name",
            "add a valid identifier here",
            struct_name,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> struct_decl.ptr = struct_name -> lexeme;
    node -> struct_decl.len = struct_name -> length;

    parser_advance(p);

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{' here",
            struct_name,
            LOC_END_OF_TOK,
            p -> file_index
        );
    }

    parser_advance(p);

    while (!parser_check(p, T_RBrace)) {
        Token* field_name = parser_peek(p);

        if (field_name -> kind != T_Ident) {
            err_ast_add(
                "expected identifier for struct field",
                "add a valid identifier here",
                field_name,
                LOC_WHOLE_TOK,
                p -> file_index
            );
        }

        parser_advance(p);

        if (!parser_check(p, T_Colon)) {
            err_ast_add(
                "expected ':' between struct field type and name",
                "add a ':' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );
        }

        parser_advance(p);

        Span field_type_span = parse_type_span(p);

        AstNode* field_default_value_expression = null;

        if (parser_check(p, T_Semi)) {
            // TODO: field_default_value_expression = parse_expression();
        }

        if (!parser_check(p, T_Semi)) {
            err_ast_add(
                "expected ';' after struct field declaration",
                "add a ';' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );
        }

        parser_advance(p);

        AstNode* field_node = ast_make_generic_blank_node(A_Field, 0, p -> file_index);

        field_node -> field.len = field_name -> length;
        field_node -> field.ptr = field_name -> lexeme;
        field_node -> field.type_token_type = field_type_span;
        field_node -> field.default_value = field_default_value_expression;
    }

    return node;
}

AstNode* parse_function_declaration(Parser* p, u16 flag) {
    AstNode* node = ast_make_function_node(flag, p -> file_index);

    Token* identifier = parser_peek(p);

    if (identifier -> kind != T_Ident) {
        err_ast_add(
            "expected identifier for function name",
            "add a valid identifier here",
            identifier,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> function_decl.ptr = identifier -> lexeme;
    node -> function_decl.len = identifier -> length;

    parser_advance(p);

    if (!parser_check(p, T_LParen)) {
        err_ast_add(
            "expected '('",
            "add a '(' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    if (!parser_check(p, T_RParen)) {
        if (!parse_parameters(p, node)) {
        }
    } else {
        parser_advance(p);
    }

    // No return type
    if (parser_check(p, T_LBrace)) {
        node -> function_decl.body = parse_block(p); 
        return node;
    }

    if (!parser_check(p, T_Colon)) {
        err_ast_add(
            "expected ':' for return type",
            "add a ':' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    parser_advance(p);

    return node;
}

bool parse_parameters(Parser* p, AstNode* node) {
    bool result = true;

    while (p -> cursor < p -> count) {
        Token* token = parser_peek(p);

        if (token -> kind == T_Comma) {
            parser_advance(p);
        } else if (token -> kind == T_RParen) {
            parser_advance(p);
            break;
        } else if (token -> kind == T_Eof || token -> kind == T_LBrace) {
            break;
        }

        Token* param_name = parser_peek(p);

        if (param_name -> kind != T_Ident) {
            err_ast_add(
                "expected parameter name",
                "add a valid identifier here",
                parser_peek(p),
                LOC_WHOLE_TOK,
                p -> file_index
            );
            recover_fn_param_decl(p);
            result = false;
            continue;
        }

        parser_advance(p);

        if (!parser_check(p, T_Colon)) {
            err_ast_add(
                "expected ':' between parameter and type",
                "add a ':' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            recover_fn_param_decl(p);
            result = false;
            continue;
        }
        
        parser_advance(p);

        Span type_span = parse_type_span(p);

        AstNode* parameter = ast_make_generic_blank_node(A_Parameter, 0, p -> file_index);

        parameter -> parameter.type_token_span = type_span;
        parameter -> parameter.ptr = param_name -> lexeme;
        parameter -> parameter.len = param_name -> length;

        ast_function_push_param(node, *parameter);

        if (parser_check(p, T_RParen)) {
            parser_advance(p);
            break;
        }

        if (parser_check(p, T_Comma)) {
            parser_advance(p);
            continue;
        }

        err_ast_add(
            "expected ',' or ')' after parameter",
            "add a ',' or ')' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );
        recover_fn_param_decl(p);
        result = false;
    }

    return result;
}
