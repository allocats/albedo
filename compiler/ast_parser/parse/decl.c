#include "../parser.h" 

#include "../../ast/ast.h" 
#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* parse_module_decl(Parser* p) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

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

    parser_advance(p);
    
    return node;
}

AstNode* parse_import_decl(Parser* p) {
    AstNode* node = arena_alloc(albedo_ctx.arena, sizeof(*node));

    node -> kind = A_Import;

    if (parser_check(p, T_StrLit)) {
        Token* token = parser_advance(p);
        
        node -> import_decl.kind = ImportRel; 
        node -> import_decl.relative.ptr = token -> lexeme + 1;
        node -> import_decl.relative.len = token -> length - 1;

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

    parser_advance(p);

    return node;
}

AstNode* parse_fn_decl(Parser* p) {
    AstNode* node = ast_make_fn_node(false);

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

    while (p -> cursor < p -> count) {
        if (parser_check(p, T_RParen)) {
            break;
        }

        if (parser_check(p, T_LBrace)) {
        }
    }

    return node;
}
