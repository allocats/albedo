#include "../parser.h"

AstNode* top_level_decl_parse_fail(Parser* p, AstNode* node) {
    node -> kind = A_ParseError;
    recover_top_level_decl(p);
    return node;
}

AstNode* stmt_parse_fail(Parser* p, AstNode* node) {
    node -> kind = A_ParseError;
    recover_stmt(p);
    return node;
}
