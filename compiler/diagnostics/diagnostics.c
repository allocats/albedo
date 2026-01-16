#include "diagnostics.h"

#include "../albedo/types.h"
#include "../lexer/types.h"
#include "../utils/ansi_codes.h"
#include "../utils/macros.h"
#include "types.h"

#include <stdio.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

void extend_diagnostics(void) {
    if (UNLIKELY(diag_ctx.diag_count >= diag_ctx.diag_capacity)) {
        usize size = sizeof(Diagnostic) * diag_ctx.diag_capacity;

        diag_ctx.diags = arena_realloc(albedo_ctx.arena, diag_ctx.diags, size, size * 2);
        diag_ctx.diag_capacity *= 2;
    }
}

/*
*   Init compiler diagnostics
*/

void err_file_not_found(const char* path) {
    fprintf(
        stderr,
        "%serror:%s %scannot find file '%s'%s\n",
        ANSI_RED,
        ANSI_RESET,
        ANSI_BOLD,
        path,
        ANSI_RESET
    );

    albedo_ctx.error_count++;
}

void err_cant_open_file(const char* path) {
    fprintf(
        stderr,
        "%serror:%s %scannot open file '%s'%s\n",
        ANSI_RED,
        ANSI_RESET,
        ANSI_BOLD,
        path,
        ANSI_RESET
    );

    albedo_ctx.error_count++;
}

void err_cant_map_file(const char* path) {
    fprintf(
        stderr,
        "%serror:%s %sfailed to map file '%s'%s\n",
        ANSI_RED,
        ANSI_RESET,
        ANSI_BOLD,
        path,
        ANSI_RESET
    );

    albedo_ctx.error_count++;
}

/*
*   Lexer diagnostics
*/

void err_unknown_token(Token* token, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = "unknown token";
    diag -> help = null;

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = token -> length;
    diag -> index = index;

    albedo_ctx.error_count++;
}

void err_unterminated_delimiter(Token* token, u32 index, DelimType type) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;

    switch (type) {
        case DELIM_STRING: {
            diag -> msg = "missing closing string delimiter '\"'";
        } break;

        case DELIM_CHAR: {
            diag -> msg = "missing closing char delimiter '''";
        } break;

        case DELIM_COMMENT: {
            diag -> msg = "missing closing block comment delimiter '*/'";
        } break;
    }

    diag -> help = "add missing closing delimiter";

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = type == DELIM_COMMENT ? 2 : 1;
    diag -> index = index;

    albedo_ctx.error_count++;
}

void err_invalid_escape_sequence(char* start, usize length, char* help, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = "invalid escape sequence";
    diag -> help = help;
    diag -> index = index;

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < start) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = length;

    albedo_ctx.error_count++;
}

void err_delim_stack_unclosed(Token* token, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = "unclosed delimiter";
    diag -> index = index;

    switch (token -> kind) {
        case T_Lt: {
            diag -> help = "add a closing '>'";
        } break;

        case T_LParen: {
            diag -> help = "add a closing ')'";
        } break;

        case T_LBrace: {
            diag -> help = "add a closing '}'";
        } break;

        case T_LBracket: {
            diag -> help = "add a closing ']'";
        } break;

        default: {
            diag -> help = "add closing delimiter";
        } break;
    }

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = token -> length;

    albedo_ctx.error_count++;
}

void err_delim_stack_max(Token* token, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = "what are you doing... delimiter stack is at max, how are you nesting this much";
    diag -> index = index;
    diag -> help = "stop what you are doing and go seek medical help";

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = token -> length;

    albedo_ctx.error_count++;
}

void err_delim_unopened(Token* token, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = "unopened delimiter";
    diag -> index = index;

    if (token -> kind == T_RParen) {
        diag -> help = "missing '('";
    } else if (token -> kind == T_RBrace) {
        diag -> help = "missing '{'";
    } else if (token -> kind == T_RBracket) {
        diag -> help = "missing '['";
    }


    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = token -> length;

    albedo_ctx.error_count++;
}

void err_delim_mismatch(Token* token, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = "mismatched delimiters";
    diag -> index = index;
    diag -> help = nullptr;

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;
    diag -> len = token -> length;

    albedo_ctx.error_count++;
}

/*
*   AST Diagnostics
*/

void err_ast_add(char* msg, char* help, Token* token, u32 loc, u32 index) {
    extend_diagnostics();

    Diagnostic* diag = &diag_ctx.diags[diag_ctx.diag_count++];

    diag -> kind = DIAG_ERR;
    diag -> msg = msg;
    diag -> help = help;
    diag -> index = index;

    u32 line = 1;
    u32 col = 1;

    const char* cursor = albedo_ctx.files[index].buffer;

    while (cursor < token -> lexeme) {
        if (*cursor == '\n') {
            line++;
            col = 1;
        } else {
            col++;
        }

        cursor++;
    }

    diag -> line = line;
    diag -> col = col;

    if (loc & LOC_START_OF_TOK) {
        diag -> len = 1;
    } else if (loc & LOC_END_OF_TOK) {
        diag -> len = 1;
        diag -> col += token -> length;
    } else if (loc & LOC_WHOLE_TOK) {
        diag -> len = token -> length;
    }

    albedo_ctx.error_count++;
}

/*
*   Diagnostics control
*/

const char* match_level_colour(DiagKind kind) {
    switch (kind) {
        case DIAG_NOTE: {
            return ANSI_BLUE;
        } break;

        case DIAG_WARN: {
            return ANSI_YELLOW;
        } break;

        case DIAG_ERR: {
            return ANSI_RED;
        } break;
    }
}

const char* match_level(DiagKind kind) {
    switch (kind) {
        case DIAG_NOTE: {
            return "note";
        } break;

        case DIAG_WARN: {
            return "warning";
        } break;

        case DIAG_ERR: {
            return "error";
        } break;
    }
}

const char* get_line_col_indent(u32 line) {
    static const char* indents[] = {
        "",
        " ",
        "  ",
        "   ",
        "    ",
        "     ",
        "      ",
        "       ",
        "        ",
        "         ",
        "          "
    };

    u32 digits = 1;

    while (line >= 10) {
        line /= 10;
        digits++;
    }

    if (UNLIKELY(digits >= sizeof(indents) / sizeof(indents[0]))) {
        return indents[sizeof(indents) / sizeof(indents[0]) - 1];
    }

    return indents[digits];
}

const char* get_source_line(const char* buffer, u32 line, u32* len) {
    u32 current_line = 1;

    const char* cursor = buffer;

    while (current_line < line) {
        if (*cursor == '\n') {
            current_line += 1;
        }

        cursor++;
    }

    const char* start = cursor;

    while (*cursor != '\n') {
        cursor++;
    }

    *len = cursor - start;

    return start;
}

void diagnostics_print(void) {
    u32 count = diag_ctx.diag_count > DIAG_SUPPRESS_THRESHOLD ? DIAG_SUPPRESS_THRESHOLD : diag_ctx.diag_count;

    for (u32 i = 0; i < count; i++) {
        Diagnostic diag = diag_ctx.diags[i];

        const char* level_colour = match_level_colour(diag.kind); 
        const char* level = match_level(diag.kind); 

        const char* path = albedo_ctx.files[diag.index].path;
        const char* buffer = albedo_ctx.files[diag.index].buffer;

        const char* line_indent = get_line_col_indent(diag.line);

        u32 source_line_length = 0;
        const char* source_line = get_source_line(buffer, diag.line, &source_line_length);

        // Header 
        fprintf(
            stderr,
            "%s%s:%s %s%s%s\n",
            level_colour,
            level,
            ANSI_RESET,
            ANSI_BOLD,
            diag.msg,
            ANSI_RESET
        );

        // Location
        fprintf(
            stderr,
            " %s%s-->%s %s:%u:%u\n",
            ANSI_MAGENTA,
            ANSI_BOLD,
            ANSI_RESET,
            path,
            diag.line,
            diag.col
        );

        // Source context
        fprintf(stderr, "%s %s|%s\n", line_indent, ANSI_BOLD, ANSI_RESET);
        fprintf(
            stderr,
            "%u %s|%s %.*s\n",
            diag.line,
            ANSI_BOLD,
            ANSI_RESET,
            source_line_length,
            source_line
        );
        fprintf(stderr, "%s %s| %s", line_indent, ANSI_BOLD, ANSI_RESET);

        u32 spaces =  diag.col - 1;
        fprintf(stderr, "%*s", spaces, "");

        fprintf(stderr, "%s%s", ANSI_GREEN, ANSI_BOLD);
        u32 caret_len = diag.len;

        for (u32 i = 0; i < caret_len; i++) {
            fprintf(stderr, "^");
        }

        fprintf(stderr, "%s", ANSI_RESET);

        if (diag.help) {
            fprintf(
                stderr,
                "%s%s help: %s%s",
                ANSI_BOLD,
                ANSI_GREEN,
                diag.help,
                ANSI_RESET
            );
        }

        fprintf(stderr, "\n");
        fprintf(stderr, "%s %s|%s\n\n", line_indent, ANSI_BOLD, ANSI_RESET);
    }
}
