#pragma once
#ifndef ALBEDO_LEXER_TYPES_H
#define ALBEDO_LEXER_TYPES_H

#include "../utils/types.h"

typedef enum {
    DELIM_STRING,
    DELIM_CHAR,
    DELIM_COMMENT,
} DelimType;

static const u8 char_map[256] = {
    ['0' ... '9'] = 1,

    ['a' ... 'z'] = 2,
    ['A' ... 'Z'] = 2,
    ['_'] = 2,
    
    ['&'] = 4,
    ['|'] = 4,
    ['~'] = 4,
    ['^'] = 4,
    ['-'] = 4,
    ['+'] = 4,
    ['/'] = 4,
    ['*'] = 4,
    ['%'] = 4,
    ['='] = 4,
    ['!'] = 4,
    ['<'] = 4,
    ['>'] = 4,
    ['.'] = 4,

    [','] = 8,
    ['['] = 8,
    [']'] = 8,
    ['('] = 8,
    [')'] = 8,
    ['{'] = 8,
    ['}'] = 8,
    [';'] = 8,
    [':'] = 8,
    ['\0'] = 8,

    [' '] = 16, 
    ['\t'] = 16, 
    ['\n'] = 16,
    ['\f'] = 16,
    ['\r'] = 16,

    ['\''] = 32,
    ['\"'] = 32,
};

#endif // !ALBEDO_LEXER_TYPES_H
