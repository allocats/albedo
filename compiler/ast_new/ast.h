#pragma once
#ifndef NEW_ALBEDO_AST_H
#define NEW_ALBEDO_AST_H

#include "types.h"

#include <stdio.h>

/* 
*  ----------------------------
*  | 1. Node making functions |
*  ----------------------------
*/ 
AstNode* ast_make_generic_blank_node(AstKind kind, u16 flags, u32 file_index);
AstNode* ast_make_function_node(u16 flags, u32 file_index);
AstNode* ast_make_struct_node(u32 file_index);
AstNode* ast_make_enum_node(u32 file_index);
AstNode* ast_make_union_node(u32 file_index);
AstNode* ast_make_call_node(u32 file_index);
AstNode* ast_make_struct_init_node(u32 file_index);
AstNode* ast_make_if_node(u32 file_index);
AstNode* ast_make_block_node(u32 file_index);

/* 
*  -------------------------------------
*  | 2. Dynamic array helper functions |
*  -------------------------------------
*/ 
void ast_enum_push_variant(AstNode* node, AstNode variant);
void ast_struct_push_field(AstNode* node, AstNode field);
void ast_function_push_param(AstNode* node, AstNode param);
void ast_block_push_stmt(AstNode* node, AstNode* stmt);

/* 
*  --------------------------------------
*  | 3. Node attribute helper functions |
*  --------------------------------------
*/ 
bool is_resolved(AstNode* node);
bool is_typed(AstNode* node);
bool is_lvalue(AstNode* node);
bool is_const(AstNode* node);
bool is_comp_time(AstNode* node);
bool is_inline(AstNode* node);
bool is_external(AstNode* node);
bool is_static(AstNode* node);
bool is_variadac(AstNode* node);

/* 
*  --------------------------------------
*  | 4. AST Dump for debugging and info |
*  --------------------------------------
*/ 
void dump_ast(FILE* fd, AST* ast);

#endif // !NEW_ALBEDO_AST_H
