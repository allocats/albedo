#pragma once
#ifndef AST_H
#define AST_H

/*
*   Simple helper functions to make creating/initialising nodes cleaner
*   during parser
*/

#include "types.h"

#define IMPORT_DEFAULT_CAP  8

#define PARAM_DEFAULT_CAP   8
#define FIELD_DEFAULT_CAP   8
#define ENUM_DEFAULT_CAP    16
#define UNION_DEFAULT_CAP   8
#define TUNION_DEFAULT_CAP  8

#define MATCH_DEFAULT_CAP   8
#define BLOCK_DEFAULT_CAP   8


/*
*   Node making function helpers
*/
AstNode* ast_make_fn_node(bool is_extern);
AstNode* ast_make_struct_node(bool is_extern);
AstNode* ast_make_enum_node(void);
AstNode* ast_make_union_node(void);
AstNode* ast_make_tunion_node(void);
AstNode* ast_make_match_node(void);
AstNode* ast_make_block_node(void);
AstNode* ast_make_fn_call_node(void);
AstNode* ast_make_struct_init_node(void);

/*
*   Initializers
*/
void ast_init_lib_import(AstNode* node);
void ast_init_module_decl(AstNode* node);

/*
*   Dynamic array helpers
*/
void ast_import_lib_push(AstNode* node, Token* token);
void ast_module_segment_push(AstNode* node, Token* token);
void ast_fn_param_push(AstNode* node, AstParam param);

#endif // !AST_H
