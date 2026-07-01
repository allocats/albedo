#include "../parser.h"

#include "../../albedo/types.h"
#include "../../ast_new/ast.h"
#include "../../diagnostics/diagnostics.h"
#include "../../hash/hash.h"
#include "../../type_system/ty.h"

extern AlbedoCtx albedo_ctx;


AstNode* parse_type_expression(Parser* p, AstNode* node) {
    AstNode* type_node = ast_make_generic_blank_node(A_TypeExpr, 0, p -> file_index);

    bool is_const = false;

    if (parser_check(p, T_Const)) {
        node -> flags &= IS_CONST;
        is_const = true;
        parser_advance(p);
    }

    Token* base_type_token = parser_peek(p);

    if (base_type_token -> kind != T_Ident) {
        err_ast_add(
            "expected identifier for base type",
            "add a valid identifier here",
            base_type_token,
            LOC_WHOLE_TOK,
            p -> file_index
        );
        
        // TODO
    }

    parser_advance(p);

    u32 pointer_depth = 0;

    while (parser_check(p, T_Star)) {
        pointer_depth += 1;
        parser_advance(p);
    }

    bool is_array = false;
    AstNode* array_size_expr = null;

    if (parser_check(p, T_LBracket)) {
        is_array = true;
        parser_advance(p);

        array_size_expr = parse_expression(p);

        if (!parser_check(p, T_RBracket)) {
            err_ast_add(
                "expected ']'",
                "add a ']' here, invalid expression inside array size",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );

            // TODO
        }
    }

    type_node -> type_expr.kind = A_Ty_Base;
    type_node -> type_expr.is_constant = is_const;

    if (is_array) {
        if (array_size_expr) {
            type_node -> type_expr.kind = A_Ty_Array;
        }
    }

    return type_node;
}
