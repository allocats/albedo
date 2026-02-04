#include "../parser.h" 

#include "../../albedo/types.h" 
#include "../../diagnostics/diagnostics.h" 

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

AstSpan parse_type_span(Parser* p) {
    u32 start = p -> cursor;

    if (parser_check(p, T_DotDotDot)) {
        parser_advance(p);
    
        if (parser_check(p, T_LBracket)) {
            while (p -> cursor < p -> count && !parser_check(p, T_RBracket)) {
                parser_advance(p);
            }

            if (!parser_check(p, T_RBracket)) {
                err_ast_add(
                    "expected closing ']'",
                    "add a ']' here",
                    parser_peek_prev(p),
                    LOC_END_OF_TOK,
                    p -> file_index
                );

                return (AstSpan) { .start_index = 0, .end_index = 0 };
            }

            parser_advance(p);
            
        }

        return (AstSpan) { .start_index = start, .end_index = p -> cursor - 1 };
    }

    if (parser_check(p, T_Const)) {
        parser_advance(p);
    }

    if (!parser_check(p, T_Ident)) {
        err_ast_add(
            "expected identifier for type",
            "add a valid identifier here",
            parser_peek(p),
            LOC_WHOLE_LINE,
            p -> file_index
        );

        return (AstSpan) { .start_index = 0, .end_index = 0 };
    }

    parser_advance(p);

    if (parser_check(p, T_Lt)) {
        parser_advance(p);

        while (p -> cursor < p -> count && !parser_check(p, T_Gt)) {
            Token* generic = parser_peek(p);

            if (generic -> kind != T_Ident) {
                err_ast_add(
                    "expected identifier for generic",
                    "add a valid identifier here",
                    parser_peek(p),
                    LOC_WHOLE_LINE,
                    p -> file_index
                );

                return (AstSpan) { .start_index = 0, .end_index = 0 };
            }

            parser_advance(p);

            if (parser_check(p, T_Gt)) {
                parser_advance(p);
                break;
            }

            if (!parser_check(p, T_Comma)) {
                err_ast_add(
                    "expected ',' or '>'",
                    "add a ',' between generics or '>' to end generic declaration",
                    parser_peek_prev(p),
                    LOC_END_OF_TOK,
                    p -> file_index
                );
                
                return (AstSpan) { .start_index = 0, .end_index = 0 };
            }

            parser_advance(p);
        }
    }

    while (parser_check(p, T_Star)) {
        parser_advance(p);
    }

    while (parser_check(p, T_LBracket)) {
        while (p -> cursor < p -> count && !parser_check(p, T_RBracket)) {
            parser_advance(p);
        }

        if (!parser_check(p, T_RBracket)) {
            err_ast_add(
                "expected closing ']'",
                "add a ']' here",
                parser_peek_prev(p),
                LOC_END_OF_TOK,
                p -> file_index
            );
            
            return (AstSpan) { .start_index = 0, .end_index = 0 };
        }

        parser_advance(p);
    }

    u32 end = p -> cursor - 1;

    return (AstSpan) { .start_index = start, .end_index = end };
}
