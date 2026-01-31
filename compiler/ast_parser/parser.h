#pragma once
#ifndef AST_PARSER_H
#define AST_PARSER_H

#include "types.h"

#include "../arena/arena.h"

void extend_parse_tree(ArenaAllocator* arenea, ParseTree* tree);

void parse_tokens(void); 

/*
*   Parsing 
*/

// precedence
Precedence get_precedence(TokenKind kind);
bool is_right_associative(Precedence prec);
bool is_unary_operator(TokenKind kind);

// spans
AstSpan parse_type_span(Parser* p); 

// namespaces 
AstNode* parse_qualified_name(Parser* p);

// declarations
AstNode* parse_module_decl(Parser* p);
AstNode* parse_import_decl(Parser* p);
AstNode* parse_fn_decl(Parser* p, bool external);
AstNode* parse_struct_decl(Parser* p, bool external);
AstNode* parse_enum_decl(Parser* p, bool external);

// statements
AstNode* parse_block(Parser* p) ;
AstNode* parse_return(Parser* p); 
AstNode* parse_var_decl(Parser* p);
AstNode* parse_const_decl(Parser* p);
AstNode* parse_defer(Parser* p); 
AstNode* parse_if_stmt(Parser* p);

AstNode* parse_loop_stmt(Parser* p);
AstNode* parse_while_loop(Parser* p);
AstNode* parse_for_loop(Parser* p);

// expressions
AstNode* parse_expression(Parser* p);

/*
*   Recovery
*/
void recover_top_level_decl(Parser* p);
void recover_fn_param_decl(Parser* p);
void recover_stmt(Parser* p);

AstNode* top_level_decl_parse_fail(Parser* p, AstNode* node);
AstNode* stmt_parse_fail(Parser* p, AstNode* node);

/*
*   Parser movement and token consumption
*/
Token* parser_peek(Parser* p);
Token* parser_advance(Parser* p);
Token* parser_peek_prev(Parser* p);
bool parser_check(Parser* p, TokenKind kind);

#endif // !AST_PARSER_H
