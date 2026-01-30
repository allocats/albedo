#include "../parser.h" 

#include "../../ast/ast.h" 
#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstNode* parse_qualified_name(Parser* p, Token* ident) {
    parser_advance(p);

    if (!parser_check(p, T_ColonColon)) {
        return ast_make_ident_node(ident);
    }

    AstNode* node = ast_make_ident_node(null);

    ast_ident_namespace_push(node, ident);

    while (parser_check(p, T_ColonColon)) {
        parser_advance(p);

        Token* namespace = parser_peek(p);

        if (namespace -> kind != T_Ident) {
            err_ast_add(
                "expected identifier after '::'",
                "add a valid identifier here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            return null;
        }

        parser_advance(p);

        ast_ident_namespace_push(node, namespace);
    }

    return node;
}
