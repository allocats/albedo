#pragma once
#ifndef ALBEDO_DIAGNOSTICS_H
#define ALBEDO_DIAGNOSTICS_H

#include "types.h"

#include "../lexer/types.h"
#include "../token/types.h"

void extend_diagnostics(void);

void err_file_not_found(const char* path);
void err_cant_open_file(const char* path);
void err_cant_map_file(const char* path);

void err_unknown_token(Token* token, u32 index);
void err_unterminated_delimiter(Token* token, u32 index, DelimType type);

void err_invalid_escape_sequence(char* start, usize length, char* help, u32 index);

void diagnostics_print(void);

#endif // !ALBEDO_DIAGNOSTICS_H
