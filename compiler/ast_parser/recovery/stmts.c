#include "../parser.h"

void recover_stmt(Parser* p) {
    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;

        if (
            kind == T_Semi
        ) {
            parser_advance(p);
            return;
        }

        if (
            kind == T_RBrace    ||
            kind == T_Eof
        ) {
            return;
        }

        parser_advance(p);
    }
}

void recover_control_stmt_condition(Parser* p) {
    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;

        if (
            kind == T_LBrace    ||
            kind == T_Eof
        ) {
            return;
        }

        parser_advance(p);
    }
}

void recover_control_stmt(Parser* p) {
    u32 depth = 1;

    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;

        if (kind == T_LBrace) {
            depth++;
        }

        if (kind == T_RBrace) {
            depth--;

            if (depth == 0) {
                return;
            }
        }

        parser_advance(p);
    }
}
