#pragma once
#ifndef ALBEDO_TOKEN_TYPES_H
#define ALBEDO_TOKEN_TYPES_H

#include "../utils/types.h"

#define X_TOKENS(X) \
    X(T_Ident)      \
                    \
    X(T_Module)     \
    X(T_Import)     \
                    \
    X(T_Private)    \
                    \
    X(T_Struct)     \
    X(T_Enum)       \
                    \
    X(T_Impl)       \
    X(T_Self)       \
                    \
    X(T_Fn)         \
                    \
    X(T_Let)        \
    X(T_Const)      \
    X(T_Static)     \
                    \
    X(T_For)        \
    X(T_In)         \
    X(T_While)      \
    X(T_Loop)       \
                    \
    X(T_If)         \
    X(T_Else)       \
    X(T_Match)      \
                    \
    X(T_Break)      \
    X(T_Continue)   \
                    \
    X(T_Return)     \
                    \
    X(T_LParen)     \
    X(T_RParen)     \
    X(T_LBrace)     \
    X(T_RBrace)     \
    X(T_LBracket)   \
    X(T_RBracket)   \
                    \
    X(T_Question)   \
                    \
    X(T_Semi)       \
    X(T_Colon)      \
    X(T_ColonColon) \
                    \
    X(T_Dot)        \
    X(T_DotDot)     \
                    \
    X(T_Comma)      \
                    \
    X(T_Bang)       \
    X(T_Eq)         \
    X(T_BangEq)     \
    X(T_EqEq)       \
                    \
    X(T_Plus)       \
    X(T_PlusEq)     \
                    \
    X(T_Minus)      \
    X(T_MinusEq)    \
                    \
    X(T_Star)       \
    X(T_StarEq)     \
                    \
    X(T_Slash)      \
    X(T_SlashEq)    \
                    \
    X(T_Percent)    \
    X(T_PercentEq)  \
                    \
    X(T_Gt)         \
    X(T_GtEq)       \
                    \
    X(T_Lt)         \
    X(T_LtEq)       \
                    \
    X(T_CondAnd)    \
    X(T_CondOr)     \
                    \
    X(T_And)        \
    X(T_AndEq)      \
                    \
    X(T_Or)         \
    X(T_OrEq)       \
                    \
    X(T_Not)        \
    X(T_NotEq)      \
                    \
    X(T_Xor)        \
    X(T_XorEq)      \
                    \
    X(T_Shl)        \
    X(T_ShlEq)      \
                    \
    X(T_Shr)        \
    X(T_ShrEq)      \
                    \
    X(T_StrLit)     \
    X(T_CharLit)    \
    X(T_FloatLit)   \
    X(T_IntLit)     \
                    \
    X(T_Unknown)    \
    X(T_Error)      \
    X(T_Eof)        \

typedef enum {
    X_TOKENS(GENERATE_ENUM)
} TokenKind;

static const char* TOKEN_KIND_STRINGS[] = {
    X_TOKENS(GENERATE_STRING)
};

typedef struct {
    TokenKind kind;
    char* lexeme;
    u32 length;
} Token;

typedef struct {
    Token* items;
    u32 count;
    u32 capacity;
} Tokens;

#endif // !ALBEDO_TEN_TYPES_H
