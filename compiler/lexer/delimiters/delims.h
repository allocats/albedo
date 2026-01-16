#pragma once
#ifndef ALBEDO_LEXER_DELIMITERS_H
#define ALBEDO_LEXER_DELIMITERS_H

#include "types.h"

void delim_stack_push(DelimStack* stack, Token* token, u32 index); 
Token* delim_stack_pop(DelimStack* stack); 
void match_delim(DelimStack* stack, Token* token, u32 index);

#endif // !ALBEDO_LEXER_DELIMITERS_H
