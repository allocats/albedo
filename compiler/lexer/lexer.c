#include "lexer.h"

#include "../albedo/types.h"
#include "../diagnostics/diagnostics.h"
#include "../token/token.h"
#include "delimiters/delims.h"
#include "delimiters/types.h"
#include "types.h"

#include <string.h>

extern AlbedoCtx albedo_ctx;

#define IS_DIGIT(c) (char_map[(unsigned char)(c)] & 1)
#define IS_ALPHA(c) (char_map[(unsigned char)(c)] & 2)
#define IS_OPERATOR(c) (char_map[(unsigned char)(c)] & 4)
#define IS_DELIMITER(c) (char_map[(unsigned char)(c)] & 8)
#define IS_WHITESPACE(c) (char_map[(unsigned char)(c)] & 16)
#define IS_LITERAL_DELIMS(c) (char_map[(unsigned char)(c)] & 32)

#define SKIP_WHITESPACE(cursor) \
    while (IS_WHITESPACE(*cursor)) { cursor++; }

#define matches(str, start, length) \
    ((length) == sizeof(str) - 1 && strncmp((str), (start), (length)) == 0)

char* lex_word(char* cursor);
char* lex_number(char* cursor);
char* lex_delim(char* cursor, DelimStack* stack, u32 index);
char* lex_op(char* cursor, u32 index);
char* lex_literal(char* cursor, u32 index);
char* lex_invalid(char* cursor, u32 index);

bool is_hex_digit(char c);

void lex_from_files(void) {
    for (u32 i = 0; i < albedo_ctx.file_count; i++) {
        FileBuffer file = albedo_ctx.files[i];

        char* cursor = file.buffer;
        char* end = file.buffer + file.size;

        DelimStack stack = {
            .top = -1,
            .delims = {0}
        };

        while (cursor < end) {
            SKIP_WHITESPACE(cursor);

            char c = *cursor;

            if (IS_ALPHA(c)) {
                cursor = lex_word(cursor);
            } else if (IS_DELIMITER(c)) {
                cursor = lex_delim(cursor, &stack, i);
            } else if (IS_DIGIT(c)) {
                cursor = lex_number(cursor);
            } else if (IS_OPERATOR(c)) {
                cursor = lex_op(cursor, i);
            } else if (IS_LITERAL_DELIMS(c)) {
                cursor = lex_literal(cursor, i);
            } else {
                cursor = lex_invalid(cursor, i);
            }
        }

        if (stack.top != -1) {
            isize count = stack.top;

            for (isize n = 0; n < count + 1; n++) {
                Token* token = stack.delims[n];
                err_delim_stack_unclosed(token, i);
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
        case 'a': {
            if (matches("as", start, length)) {
                token -> kind = T_As;
                break;
            }

            token -> kind = T_Ident;
        } break;

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

            if (matches("extern", start, length)) {
                token -> kind = T_Extern;
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

            if (matches("false", start, length)) {
                token -> kind = T_False;
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
            if (matches("match", start, length)) {
                token -> kind = T_Match;
                break;
            }

            if (matches("module", start, length)) {
                token -> kind = T_Module;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'n': {
            if (matches("null", start, length)) {
                token -> kind = T_Null;
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
            if (matches("self", start, length)) {
                token -> kind = T_Self;
                break;
            }

            if (matches("struct", start, length)) {
                token -> kind = T_Struct;
                break;
            }

            if (matches("static", start, length)) {
                token -> kind = T_Static;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 't': {
            if (matches("true", start, length)) {
                token -> kind = T_True;
                break;
            }

            if (matches("test", start, length)) {
                token -> kind = T_Test;
                break;
            }

            if (matches("tunion", start, length)) {
                token -> kind = T_Tunion;
                break;
            }

            token -> kind = T_Ident;
        } break;

        case 'u': {
            if (matches("union", start, length)) {
                token -> kind = T_Union;
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
    Tokens* tokens = &albedo_ctx.tokens;

    Token* token = &tokens -> items[tokens -> count++];

    char* start = cursor;

    bool is_float = false;

    while (IS_DIGIT(*cursor) || *cursor == '_') {
        cursor++;
    }

    if (*cursor == '.' && IS_DIGIT(*(cursor + 1))) {
        is_float = true;
        cursor++;

        while (IS_DIGIT(*cursor)) {
            cursor++;
        }
    }

    token -> kind = is_float ? T_FloatLit : T_IntLit;
    token -> lexeme = start;
    token -> length = cursor - start;

    return cursor;
}

char* lex_op(char* cursor, u32 index) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);
    Tokens* tokens = &albedo_ctx.tokens;

    Token* token = &tokens -> items[tokens -> count++];

    char* start = cursor++;

    switch (*start) {
        case '+': {
            if (*cursor == '=') {
                token -> kind = T_PlusEq;
                cursor++;
                break;
            }

            token -> kind = T_Plus;
        } break;

        case '-': {
            if (*cursor == '=') {
                token -> kind = T_MinusEq;
                cursor++;
                break;
            }

            if (*cursor == '>') {
                token -> kind = T_Arrow;
                cursor++;
                break;
            }

            token -> kind = T_Minus;
        } break;

        case '*': {
            if (*cursor == '=') {
                token -> kind = T_StarEq;
                cursor++;
                break;
            }

            token -> kind = T_Star;
        } break;

        case '/': {
            if (*cursor == '/') {
                tokens -> count--;

                while (*cursor != 0 && *cursor != '\n') {
                    cursor++;
                }

                cursor++;

                break;
            }

            if (*cursor == '*') {
                tokens -> count--;

                cursor++;

                u32 depth = 1;

                while (*cursor != 0 && depth > 0) {
                    if (*cursor == '/' && *(cursor + 1) == '*') {
                        depth++;
                        cursor += 2;
                    } else if (*cursor == '*' && *(cursor + 1) == '/') {
                        depth--;
                        cursor += 2;
                    } else {
                        cursor++;
                    }
                }

                if (depth > 0) {
                    err_unterminated_delimiter(token, index, DELIM_COMMENT);
                    token -> kind = T_Error;
                    return cursor;
                }
            }

            if (*cursor == '=') {
                token -> kind = T_SlashEq;
                cursor++;
                break;
            }

            token -> kind = T_Slash;
        } break;

        case '%': {
            if (*cursor == '=') {
                token -> kind = T_PercentEq;
                cursor++;
                break;
            }

            token -> kind = T_Percent;
        } break;

        case '!': {
            if (*cursor == '=') {
                token -> kind = T_BangEq;
                cursor++;
                break;
            }

            token -> kind = T_Bang;
        } break;

        case '~': {
            if (*cursor == '=') {
                token -> kind = T_NotEq;
                cursor++;
                break;
            }

            token -> kind = T_Not;
        } break;

        case '&': {
            if (*cursor == '=') {
                token -> kind = T_AndEq;
                cursor++;
                break;
            } 

            if (*cursor == '&') {
                token -> kind = T_CondAnd;
                cursor++;
                break;
            }

            token -> kind = T_And;
        } break;

        case '|': {
            if (*cursor == '=') {
                token -> kind = T_OrEq;
                cursor++;
                break;
            } 

            if (*cursor == '|') {
                token -> kind = T_CondOr;
                cursor++;
                break;
            }

            token -> kind = T_Or;
        } break;

        case '^': {
            if (*cursor == '=') {
                token -> kind = T_XorEq;
                cursor++;
                break;
            } 

            token -> kind = T_Xor;
        } break;

        case '>': {
            if (*cursor == '=') {
                token -> kind = T_GtEq;
                cursor++;
                break;
            }

            if (*cursor == '>') {
                token -> kind = T_Shr;
                cursor++;

                if (*cursor == '=') {
                    token -> kind = T_ShrEq;
                    cursor++;
                }
                
                break;
            }

            token -> kind = T_Gt;
        } break;

        case '<': {
            if (*cursor == '=') {
                token -> kind = T_LtEq;
                cursor++;
                break;
            }

            if (*cursor == '<') {
                token -> kind = T_Shl;
                cursor++;

                if (*cursor == '=') {
                    token -> kind = T_ShlEq;
                    cursor++;
                }
                
                break;
            }

            token -> kind = T_Lt;
        } break;

        case '.': {
            if (*cursor == '.') {
                token -> kind = T_DotDot;
                cursor++;
                break;
            }
            
            token -> kind = T_Dot;
        } break;
    }

    token -> lexeme = start;
    token -> length = cursor - start;

    return cursor;
}

char* lex_delim(char* cursor, DelimStack* stack, u32 index) {
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
            delim_stack_push(stack, token, index);
        } break;

        case ')': {
            token -> kind = T_RParen;
            match_delim(stack, token, index);
        } break;

        case '[': {
            token -> kind = T_LBracket;
            delim_stack_push(stack, token, index);
        } break;

        case ']': {
            token -> kind = T_RBracket;
            match_delim(stack, token, index);
        } break;

        case '{': {
            token -> kind = T_LBrace;
            delim_stack_push(stack, token, index);
        } break;

        case '}': {
            token -> kind = T_RBrace;
            match_delim(stack, token, index);
        } break;

        case '\0': {
            token -> kind = T_Eof;
        } break;
    }

    return cursor;
}

char* lex_literal(char* cursor, u32 index) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    Tokens* tokens = &albedo_ctx.tokens;

    Token* token = &tokens -> items[tokens -> count++];

    char* start = cursor;
    
    token -> lexeme = start;

    char delim = *start;

    cursor++;

    bool has_error = false;

    while (*cursor != '\0' && *cursor != delim) {
        if (*cursor == '\\') {
            char* seq_start = cursor;
            usize seq_length = 1;

            cursor++;

            switch (*cursor) {
                case 'n':
                case 'r':
                case 't':
                case '\\':
                case '\'':
                case '\"':
                case '0': {
                    cursor++; 
                } break;

                case 'x': {
                    cursor++;

                    if (!is_hex_digit(*cursor) || !is_hex_digit(*(cursor + 1))) {
                        seq_length = (cursor + 1) - seq_start + 1;

                        err_invalid_escape_sequence(
                            seq_start,
                            seq_length,
                            "add a valid hex number here",
                            index
                        );

                        has_error = true;
                    } else {
                        cursor += 2;
                    }
                } break;

                case 'u': {
                    cursor++;

                    if (*cursor != '{') {
                        err_invalid_escape_sequence(
                            seq_start + 2,
                            seq_length,
                            "add an opening '{' here",
                            index
                        );

                        has_error = true;
                    } else {
                        cursor++;

                        u32 hex_count = 0;

                        while (is_hex_digit(*cursor) && hex_count < 6) {
                            cursor++;
                            hex_count++;
                        }

                        seq_length = cursor - seq_start;

                        if (*cursor != '}') {
                            err_invalid_escape_sequence(
                                seq_start + seq_length,
                                1,
                                "add a closing '}' at the end of the sequence",
                                index
                            );

                            has_error = true;
                        } else if (hex_count == 0) {
                            err_invalid_escape_sequence(
                                seq_start + seq_length,
                                1,
                                "add escape sequence code",
                                index
                            );

                            has_error = true;
                        } else {
                            cursor++;
                        }
                    }
                } break;

                default: {
                    cursor++;
                    seq_length = cursor - seq_start;
                    err_invalid_escape_sequence(seq_start, seq_length, null, index);
                    has_error = true;
                } break;
            }
        } else if (*cursor == '\n' || *cursor == '\r') {
            // emit error: new line in string
            has_error = true;
            break;
        } else {
            cursor++;
        }
    }


    if (*cursor != delim) {
        DelimType type = delim == '\'' ? DELIM_CHAR : DELIM_STRING;
        err_unterminated_delimiter(token, index, type);
        has_error = true;
    } else {
        cursor++;

        usize lit_length = (cursor - 1) - (start + 1);

        if (delim == '\'') {
            if (lit_length == 0) {
                // emit error: empty literal
                has_error = true;
            } else if (lit_length > 1 && *(start + 1) != '\\') {
                // emit error: invalid char
                has_error = true;
            }
        }
    }

    usize length = cursor - start;

    token -> kind = delim == '"' ? T_StrLit : T_CharLit;
    token -> length = length;

    if (has_error) {
        token -> kind = T_Error;
    }

    return cursor;
}

char* lex_invalid(char* cursor, u32 index) {
    extend_tokens(albedo_ctx.arena, &albedo_ctx.tokens);

    Tokens* tokens = &albedo_ctx.tokens;

    char* start = cursor;

    while (
        !IS_ALPHA(*cursor)            &&
        !IS_DIGIT(*cursor)            &&
        !IS_OPERATOR(*cursor)         &&
        !IS_LITERAL_DELIMS(*cursor)   &&
        !IS_DELIMITER(*cursor)        &&
        !IS_WHITESPACE(*cursor)
    ) {
        cursor++;
    }

    Token* token = &tokens -> items[tokens -> count++];

    token -> kind = T_Unknown;
    token -> lexeme = start;
    token -> length = cursor - start;

    err_unknown_token(token, index);

    return cursor;
}

inline bool is_hex_digit(char c) {
    return (c >= '0' && c <= '9') || 
           (c >= 'a' && c <= 'f') || 
           (c >= 'A' && c <= 'F');
}
