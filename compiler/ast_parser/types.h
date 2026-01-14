#pragma once
#ifndef ALBEDO_AST_PARSER_TYPES_H
#define ALBEDO_AST_PARSER_TYPES_H

#include "../ast/types.h"
#include "../utils/types.h"

typedef struct {
    usize file_index;
    usize cursor; 
    usize count;
} Parser;

typedef struct {
    AstNode** nodes;
    usize count;
    usize capacity;
} ParseTree;

#endif // !ALBEDO_AST_PARSER_TYPES_H
