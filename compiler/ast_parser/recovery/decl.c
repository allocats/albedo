#include "../parser.h"

void recover_top_level_decl(Parser* p) {
    TokenKind kind = parser_peek(p) -> kind;

    while (
        kind != T_Module    &&
        kind != T_Import    &&
        kind != T_Struct    &&
        kind != T_Enum      &&
        kind != T_Union     &&
        kind != T_Tunion    &&
        kind != T_Fn        &&
        kind != T_Eof
    ) {
        kind = parser_advance(p) -> kind;
    }
}
