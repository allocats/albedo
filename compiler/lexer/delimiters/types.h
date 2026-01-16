#pragma once
#ifndef ALBEDO_LEXER_DELIMITERS_TYPES_H
#define ALBEDO_LEXER_DELIMITERS_TYPES_H

#include "../../token/types.h"

#define MAX_DELIM_STACK 4096

typedef struct {
    isize top;
    Token* delims[MAX_DELIM_STACK];
} DelimStack;

#endif // !ALBEDO_LEXER_DELIMITERS_TYPES_H
