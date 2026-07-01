#include "../parser.h"

#include "../../diagnostics/diagnostics.h"

Span parse_type_span(Parser* p) {
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

                return (Span) { .start = 0, .end = 0 };
            }

            parser_advance(p);
            
        }

        return (Span) { .start = start, .end = p -> cursor - 1 };
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

        return (Span) { .start = 0, .end = 0 };
    }

    parser_advance(p);

    // if (parser_check(p, T_Lt)) {
    //     parser_advance(p);
    //
    //     while (p -> cursor < p -> count && !parser_check(p, T_Gt)) {
    //         Token* generic = parser_peek(p);
    //
    //         if (generic -> kind != T_Ident) {
    //             err_ast_add(
    //                 "expected identifier for generic",
    //                 "add a valid identifier here",
    //                 parser_peek(p),
    //                 LOC_WHOLE_LINE,
    //                 p -> file_index
    //             );
    //
    //             return (Span) { .start = 0, .end = 0 };
    //         }
    //
    //         parser_advance(p);
    //
    //         if (parser_check(p, T_Gt)) {
    //             parser_advance(p);
    //             break;
    //         }
    //
    //         if (!parser_check(p, T_Comma)) {
    //             err_ast_add(
    //                 "expected ',' or '>'",
    //                 "add a ',' between generics or '>' to end generic declaration",
    //                 parser_peek_prev(p),
    //                 LOC_END_OF_TOK,
    //                 p -> file_index
    //             );
    //
    //             return (Span) { .start = 0, .end = 0 };
    //         }
    //
    //         parser_advance(p);
    //     }
    // }

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
            
            return (Span) { .start = 0, .end = 0 };
        }

        parser_advance(p);
    }

    u32 end = p -> cursor - 1;

    return (Span) { .start = start, .end = end };
}
