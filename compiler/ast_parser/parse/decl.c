#include "../parser.h" 

#include "../../ast/ast.h" 
#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* parse_module_decl(Parser* p) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> span.start = p -> cursor - 1;

    if (!parser_check(p, T_Ident)) {
        err_ast_add(
            "expected identifier",
            "add an identifier here",
            parser_peek(p),
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> kind = A_Module;

    ast_init_module_decl(node);

    while (p -> cursor < p -> count) {
        Token* segment = parser_peek(p);

        if (segment -> kind != T_Ident) {
            err_ast_add(
                "expected identifier",
                "add an identifier here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return top_level_decl_parse_fail(p, node);
        }

        ast_module_segment_push(node, segment);

        parser_advance(p);
        
        Token* token = parser_peek(p);

        if (token -> kind == T_Semi) {
            break;
        }

        if (token -> kind != T_ColonColon) {
            err_ast_add(
                "expected '::' or ';' after segment",
                "add '::' or ';' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return top_level_decl_parse_fail(p, node);
        }

        parser_advance(p);
    }

    node -> span.end = p -> cursor;

    parser_advance(p);
    
    return node;
}

AstNode* parse_import_decl(Parser* p) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));
    
    node -> span.start = p -> cursor - 1;

    node -> kind = A_Import;

    if (parser_check(p, T_StrLit)) {
        Token* token = parser_advance(p);
        
        node -> import_decl.kind = ImportRel; 
        node -> import_decl.relative.ptr = token -> lexeme + 1;
        node -> import_decl.relative.len = token -> length - 2;

        return node;
    }

    if (!parser_check(p, T_Ident)) {
        err_ast_add(
            "expected identifier",
            "add an identifier here",
            parser_peek(p),
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> import_decl.kind = ImportLib;

    ast_init_lib_import(node);

    while (p -> cursor < p -> count) {
        Token* segment = parser_peek(p);

        if (segment -> kind != T_Ident) {
            err_ast_add(
                "expected identifier",
                "add an identifier here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return top_level_decl_parse_fail(p, node);
        }

        ast_import_lib_push(node, segment);

        parser_advance(p);
        
        Token* token = parser_peek(p);

        if (token -> kind == T_Semi) {
            break;
        }

        if (token -> kind != T_ColonColon) {
            err_ast_add(
                "expected '::' or ';' after segment",
                "add '::' or ';' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return top_level_decl_parse_fail(p, node);
        }

        parser_advance(p);
    }

    node -> span.end = p -> cursor;

    parser_advance(p);

    return node;
}

bool parse_generic_params(Parser* p, AstNode* node) {
    while (p -> cursor < p -> count) {
        Token* param = parser_peek(p);

        if (param -> kind != T_Ident) {
            err_ast_add(
                "expected identifier",
                "add a valid identifier here",
                parser_peek(p),
                LOC_WHOLE_TOK,
                p -> file_index
            );
            // recover
            return false;
        }

        ast_fn_generic_push(node, param);

        parser_advance(p);

        if (parser_check(p, T_Gt)) {
            parser_advance(p);
            return true;
        }

        if (parser_check(p, T_Comma)) {
            parser_advance(p);
        } else {
            err_ast_add(
                "expected ',' or '>'",
                "add a ',' between generics or '>' to end generics declaration",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );
            // recover
            return false;
        }
    }

    return false;
}

bool parse_params(Parser* p, AstNode* node) {
    if (parser_check(p, T_RParen)) {
        parser_advance(p);
        return true;
    }

    bool result = true;

    while (p -> cursor < p -> count) {
        Token* tok = parser_peek(p);
        
        if (tok -> kind == T_Comma) {
            parser_advance(p);
        } else if (tok -> kind == T_RParen) {
            parser_advance(p);
            break;
        } else if (tok -> kind == T_Eof || tok -> kind == T_LBrace) {
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

        AstSpan type_span = parse_type_span(p);

        AstParam param = (AstParam) {
            .name_ptr = param_name -> lexeme,
            .name_len = param_name -> length,
            .type = type_span
        };

        ast_fn_param_push(node, param);

        if (parser_check(p, T_RParen)) {
            parser_advance(p);
            return result;
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

AstNode* parse_fn_decl(Parser* p) {
    AstNode* node = ast_make_fn_node(false);

    node -> span.start = p -> cursor - 1;

    if (parser_check(p, T_Lt)) {
        parser_advance(p);

        if (!parse_generic_params(p, node)) {
            return top_level_decl_parse_fail(p, node);
        }
    }

    Token* name = parser_peek(p);

    if (name -> kind != T_Ident) {
        err_ast_add(
            "expected identifier for function name",
            "add an identifier here",
            name,
            LOC_WHOLE_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> function_decl.name_ptr = name -> lexeme;
    node -> function_decl.name_len = name -> length;

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

    parser_advance(p);

    parse_params(p, node);

    if (parser_check(p, T_Colon)) {
        parser_advance(p);
        node -> function_decl.return_type = parse_type_span(p);
    }

    if (!parser_check(p, T_LBrace)) {
        err_ast_add(
            "expected '{'",
            "add a '{' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );

        return top_level_decl_parse_fail(p, node);
    }

    node -> function_decl.body = parse_block(p);

    node -> span.end = p -> cursor - 1;

    return node;
}
