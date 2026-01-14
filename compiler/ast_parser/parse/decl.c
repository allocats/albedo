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
    }

    Token* token = parser_advance(p);

    node -> kind = A_Module;
    node -> module_decl.path_ptr = token -> lexeme;
    node -> module_decl.path_len = token -> length;

    if (!parser_check(p, T_Semi)) {
        err_ast_add(
            "expected ';'",
            "add a ';' here",
            parser_peek_prev(p),
            LOC_END_OF_TOK,
            p -> file_index
        );
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
    }

    node -> import_decl.kind = ImportLib;

    ast_init_lib_import(node);

    while (p -> cursor < p -> count) {
        Token* module = parser_advance(p);

        if (module -> kind != T_Ident) {
            err_ast_add(
                "expected identifier",
                "add an identifier here",
                parser_peek(p),
                LOC_WHOLE_TOK,
                p -> file_index
            );
            break;
        }

        ast_import_lib_push(node, module);

        if (parser_check(p, T_Eof)) {
            err_ast_add(
                "expected ';'",
                "add a ';' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return node;
        }

        if (parser_check(p, T_Semi)) {
            break;
        }

        if (!parser_check(p, T_ColonColon)) {
            err_ast_add(
                "expected '::' between segments",
                "add a '::' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );
            break;
        }
    }

    parser_advance(p);

    return node;
}
