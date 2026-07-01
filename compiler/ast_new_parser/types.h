#pragma once
#ifndef NEW_ALBEDO_PARSER_TYPES_H
#define NEW_ALBEDO_PARSER_TYPES_H

#include "../utils/types.h"

typedef struct {
    u32 file_index;
    usize cursor;
    usize count;
} Parser;

typedef enum {
    PREC_NONE           = 0,
    PREC_ASSIGNMENT     = 1,  // =, +=, -=, *=, /=, %=, <<=, >>=, &=, ^=, |=
    PREC_CAST           = 2,  // as
    PREC_LOGICAL_OR     = 3,  // ||
    PREC_LOGICAL_AND    = 4,  // &&
    PREC_BITWISE_OR     = 5,  // |
    PREC_BITWISE_XOR    = 6,  // ^
    PREC_BITWISE_AND    = 7,  // &
    PREC_EQUALITY       = 8,  // ==, !=
    PREC_COMPARISON     = 9,  // <, <=, >, >=
    PREC_RANGE          = 10, // ..
    PREC_SHIFT          = 11, // <<, >>
    PREC_ADDITIVE       = 12, // +, -
    PREC_MULTIPLICATIVE = 13, // *, /, %
    PREC_UNARY          = 14, // !, ~, *, &
    PREC_POSTFIX        = 15, // (), [], ., ->
    PREC_PRIMARY        = 16, // literals, identifiers, ()
} Precedence;

#endif // !NEW_ALBEDO_PARSER_TYPES_H
