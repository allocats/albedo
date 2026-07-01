#include "../parser.h"

void recover_top_level_decl(Parser* p) {
    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;

        if (
            kind == T_Module    ||
            kind == T_Import    ||
            kind == T_Struct    ||
            kind == T_Enum      ||
            kind == T_Union     ||
            kind == T_Tunion    ||
            kind == T_Fn        ||
            kind == T_Eof
        ) {
            return;
        }

        parser_advance(p);
    }
}

void recover_fn_param_decl(Parser* p) {
    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;

        if (
            kind == T_Eof       ||
            kind == T_Comma     ||
            kind == T_LBrace    ||
            kind == T_RParen
        ) {
            return;
        }

        parser_advance(p);
    }
}

void recover_enum_variant(Parser* p) {
    while (p -> cursor < p -> count) {
        TokenKind kind = parser_peek(p) -> kind;
        
        if (kind == T_Semi) {
            parser_advance(p);
            return;
        }

        if (
            kind == T_Eof       ||
            kind == T_RBrace
        ) {
            return;
        }

        parser_advance(p);
    }
}
