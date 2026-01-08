#include "lexer.h"

#include "../albedo/albedo.h"
#include "../token/token.h"

#include <string.h>

extern AlbedoCtx albedo_ctx;

#define IS_DIGIT(c) (char_map[(unsigned char)(c)] & 1)
#define IS_ALPHA(c) (char_map[(unsigned char)(c)] & 2)
#define IS_OPERATOR(c) (char_map[(unsigned char)(c)] & 4)
#define IS_DELIMITER(c) (char_map[(unsigned char)(c)] & 8)
#define IS_WHITESPACE(c) (char_map[(unsigned char)(c)] & 16)
#define IS_STRING_DELIMS(c) (char_map[(unsigned char)(c)] & 32)

#define SKIP_WHITESPACE(cursor) \
    while (IS_WHITESPACE(*cursor)) cursor++

#define matches(str, start, length) \
    ((length) == sizeof(str) - 1 && strncmp((str), (start), (length)) == 0)

char* lex_word(char* cursor);
char* lex_number(char* cursor);
char* lex_op(char* cursor);
char* lex_delim(char* cursor);
char* lex_invalid(char* cursor);

void lex_from_files(void) {
    for (u32 i = 0; albedo_ctx.file_count; i++) {
        FileBuffer file = albedo_ctx.files[i];

        char* cursor = file.buffer;
        char* end = file.buffer + file.size;
        
        while (cursor < end) {
            SKIP_WHITESPACE(cursor);

            char c = *cursor;

            if (IS_ALPHA(c)) {
                cursor = lex_word(cursor);
            } else if (IS_DELIMITER(c)) {
                cursor = lex_delim(cursor);
            } else if (IS_DIGIT(c)) {
                cursor = lex_number(cursor);
            } else if (IS_OPERATOR(c)) {
                cursor = lex_op(cursor);
            } else {
                cursor = lex_invalid(cursor);
            }
        }
    }
}

char* lex_word(char* cursor) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    Tokens* tokens = &albedo_ctx.tokens;

    char* start = cursor;

    while (IS_ALPHA(*cursor) || IS_DIGIT(*cursor)) {
        cursor++;
    }

    u32 length = cursor - start;

    Token* token = &tokens -> items[tokens -> count++];

    token -> lexeme = start;
    token -> length = length;

    switch (*start) {
        case 'b': {
            if (matches("break", start, length)) {
                token -> kind = T_Break;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'c': {
            if (matches("const", start, length)) {
                token -> kind = T_Const;
                break;
            }

            if (matches("continue", start, length)) {
                token -> kind = T_Continue;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'e': {
            if (matches("else", start, length)) {
                token -> kind = T_Else;
                break;
            }

            if (matches("enum", start, length)) {
                token -> kind = T_Enum;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'f': {
            if (matches("fn", start, length)) {
                token -> kind = T_Fn;
                break;
            }

            if (matches("for", start, length)) {
                token -> kind = T_For;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'i': {
            if (matches("in", start, length)) {
                token -> kind = T_In;
                break;
            }

            if (matches("if", start, length)) {
                token -> kind = T_If;
                break;
            }

            if (matches("import", start, length)) {
                token -> kind = T_Import;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'l': {
            if (matches("let", start, length)) {
                token -> kind = T_Let;
                break;
            }

            if (matches("loop", start, length)) {
                token -> kind = T_Loop;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'm': {
            if (matches("module", start, length)) {
                token -> kind = T_Module;
                break;
            }

            if (matches("match", start, length)) {
                token -> kind = T_Match;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'p': {
            if (matches("private", start, length)) {
                token -> kind = T_Private;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'r': {
            if (matches("return", start, length)) {
                token -> kind = T_Return;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 's': {
            if (matches("static", start, length)) {
                token -> kind = T_Static;
                break;
            }

            if (matches("struct", start, length)) {
                token -> kind = T_Struct;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'w': {
            if (matches("while", start, length)) {
                token -> kind = T_While;
                break;
            }

            token -> kind = T_Ident;
        } break;

        default: {
            token -> kind = T_Ident;
        } break;
    }

    return cursor;
}

char* lex_number(char* cursor) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    return cursor;
}

char* lex_op(char* cursor) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    return cursor;
}

char* lex_delim(char* cursor) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    Tokens* tokens = &albedo_ctx.tokens;

    char* start = cursor++;

    u32 length = cursor - start;

    Token* token = &tokens -> items[tokens -> count++];

    token -> lexeme = start;
    token -> length = length;

    switch (*start) {
        case ',': {
            token -> kind = T_Comma;
        } break;

        case ';': {
            token -> kind = T_Semi;
        } break;

        case ':': {
            token -> kind = T_Colon;

            if (*cursor == ':') {
                token -> kind = T_ColonColon;
                token -> length = 2;
                cursor++;
                break;
            }
        } break;

        case '(': {
            token -> kind = T_LParen;
        } break;

        case ')': {
            token -> kind = T_RParen;
        } break;

        case '[': {
            token -> kind = T_LBracket;
        } break;

        case ']': {
            token -> kind = T_RBracket;
        } break;

        case '{': {
            token -> kind = T_LBrace;
        } break;

        case '}': {
            token -> kind = T_RBrace;
        } break;

        case '\0': {
            token -> kind = T_Eof;
        } break;
    }

    return cursor;
}

char* lex_invalid(char* cursor) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    Tokens* tokens = &albedo_ctx.tokens;

    char* start = cursor;

    char c = *cursor++;

    while (
        !IS_ALPHA(c)            &&
        !IS_DIGIT(c)            &&
        !IS_OPERATOR(c)         &&
        !IS_STRING_DELIMS(c)    &&
        !IS_DELIMITER(c)        &&
        !IS_WHITESPACE(c)
    ) {
        c = (*cursor)++;
    }

    Token* token = &tokens -> items[tokens -> count++];

    token -> kind = T_Unknown;
    token -> lexeme = start;
    token -> length = cursor - start;

    // emit error

    return cursor;
}
