#pragma once
#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "types.h"

#include "../arena/arena.h"

void extend_parse_tree(ArenaAllocator* arenea, ParseTree* tree);

void parse_tokens(void); 

AstNode* parse_module_decl(Parser* p);
AstNode* parse_import_decl(Parser* p);

Token* parser_peek(Parser* p);
Token* parser_advance(Parser* p);
Token* parser_peek_prev(Parser* p);
bool parser_check(Parser* p, TokenKind kind);

#endif // !AST_PARSER_H
