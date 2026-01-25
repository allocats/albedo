#pragma once
#ifndef ALBEDO_TOKEN_H
#define ALBEDO_TOKEN_H

#include "../arena/arena.h"
#include "types.h"

#define TOKENS_GROWTH_FACTOR 2 

void print_token(Token* token);
void print_tokens(Tokens tokens);
void extend_tokens(ArenaAllocator* arena, Tokens* tokens);

#endif // !ALBEDO_TOKEN_H
