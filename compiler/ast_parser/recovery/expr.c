#include "../parser.h"

void recover_struct_init(Parser* p) {
    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;

        if (
            kind == T_Comma
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
