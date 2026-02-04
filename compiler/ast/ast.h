#pragma once
#ifndef AST_H
#define AST_H

/*
*   Simple helper functions to make creating/initialising nodes cleaner
*   during parser
*/

#include "types.h"

#include "../ast_parser/types.h"

#include <stdio.h>

#define IMPORT_DEFAULT_CAP  8

#define PARAM_DEFAULT_CAP   8
#define FIELD_DEFAULT_CAP   8
#define ENUM_DEFAULT_CAP    16
#define UNION_DEFAULT_CAP   8
#define TUNION_DEFAULT_CAP  8

#define MATCH_DEFAULT_CAP   8
#define BLOCK_DEFAULT_CAP   8

#define LIST_DEFAULT_CAP    32

/*
*   Node making function helpers
*/
AstNode* ast_make_fn_node(bool is_extern);
AstNode* ast_make_struct_node(bool is_extern);
AstNode* ast_make_enum_node(bool is_extern);
AstNode* ast_make_union_node(void);
AstNode* ast_make_tunion_node(void);
AstNode* ast_make_if_node(void);
AstNode* ast_make_match_node(void);
AstNode* ast_make_block_node(void);
AstNode* ast_make_return_node(AstNode* expr);
AstNode* ast_make_defer_node(AstNode* stmt);
AstNode* ast_make_fn_call_node(AstNode* ident);
AstNode* ast_make_struct_init_node(AstNode* identifier);

AstNode* ast_make_loop_node(void);
AstNode* ast_make_while_node(void);
AstNode* ast_make_for_node(void);

AstNode* ast_make_index_node(AstNode* ident, AstNode* index);
AstNode* ast_make_member_access_node(AstNode* ident, Token* field);

AstNode* ast_make_literal_node(Token* token);
AstNode* ast_make_ident_node(Token* token);
AstNode* ast_make_assign_node(AstNode* target, AstNode* value, TokenKind op);
AstNode* ast_make_binop_node(AstNode* left, AstNode* right, TokenKind op); 
AstNode* ast_make_cast_node(AstNode* expr, AstSpan type); 
AstNode* ast_make_unary_node(AstNode* operand, TokenKind op); 

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
void ast_fn_generic_push(AstNode* node, Token* token);
void ast_fn_call_arg_push(AstNode* node, AstNode* arg);

void ast_struct_generic_push(AstNode* node, Token* token);
void ast_struct_field_push(AstNode* node, AstField field);
void ast_struct_init_field_push(AstNode* node, AstFieldInit field);

void ast_enum_variant_push(AstNode* node, AstEnumVariant variant);

void ast_block_stmt_push(AstNode* node, AstNode* stmt);
void ast_if_branch_push(AstNode* node, AstNode* condition, AstNode* block);

void ast_ident_namespace_push(AstNode* node, Token* token);

/*
*   AstNodeList functions
*/
void ast_node_list_init(AstNodeList* list);
void ast_node_list_push(AstNodeList* list, AstNode* node);

/*
*   Debugging
*/
void print_ast(FILE* fd, ParseTree* tree, Tokens* tokens);

#endif // !AST_H
