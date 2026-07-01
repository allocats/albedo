#pragma once
#ifndef NEW_ALBEDO_AST_PARSER_H
#define NEW_ALBEDO_AST_PARSER_H

#include "types.h"

#include "../ast_new/types.h"
#include "../token/types.h"

/*
 *  ===========================
 *  | 1. Entry parse function |
 *  ===========================
 */
void parse_tokens();

/*
 *  ====================================
 *  | 2. Parsing and building that AST |
 *  ====================================
 */
AstNode* parse_import_declaration(Parser* p);
AstNode* parse_struct_declaration(Parser* p);
AstNode* parse_enum_declaration(Parser* p);
AstNode* parse_function_declaration(Parser* p, u16 flag);

AstNode* parse_expression(Parser* p);

AstNode* parse_block(Parser* p);

Span parse_type_span(Parser* p);

/*
 *  ==============================
 *  | 3. Parse failure functions |
 *  ==============================
 */
AstNode* top_level_decl_parse_fail(Parser* p, AstNode* node);

/*
 *  =========================
 *  | 4. Recovery functions |
 *  =========================
 */
void recover_top_level_decl(Parser* p);
void recover_fn_param_decl(Parser* p);

/*
 *  ==========================================
 *  | 5. Parser movement / Token shenanigans |
 *  ==========================================
 */
Token* parser_peek(Parser* p); 
Token* parser_peek_prev(Parser* p); 
Token* parser_advance(Parser* p); 
bool  parser_check(Parser* p, TokenKind kind); 

#endif // !NEW_ALBEDO_AST_PARSER_H
