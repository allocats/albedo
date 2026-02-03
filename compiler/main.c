#include <stdio.h>
#include <unistd.h>

#include "albedo/albedo.h"
#include "albedo/types.h"
#include "ast_parser/parser.h"
#include "arena/arena.h"
#include "buffers/buffers.h"
#include "diagnostics/diagnostics.h"
#include "lexer/lexer.h"
#include "utils/ansi_codes.h"
#include "utils/types.h"

static ArenaAllocator arena = {0};

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

i32 main(i32 argc, char* argv[]) {
    init_ansi_codes();

    if (argc < 2) {
        fprintf(
            stderr,
            "%serror:%s %sno input files%s\n",
            ANSI_RED,
            ANSI_RESET,
            ANSI_BOLD,
            ANSI_RESET
        );

        return 1;
    }

    init_arena(&arena, 66536);
    init_compiler(&arena, argc, argv);

    if (albedo_ctx.file_count == 0) {
        fprintf(
            stderr,
            "%serror:%s %sno input files%s\n",
            ANSI_RED,
            ANSI_RESET,
            ANSI_BOLD,
            ANSI_RESET
        );
    }

    lex_from_files();

    if (albedo_ctx.error_count > 0) {
        goto compiler_exit;
    }

    parse_tokens();

    #ifdef DEBUG_MODE
    #include "ast/ast.h"
    #include "token/token.h"

    print_tokens(albedo_ctx.tokens);
    print_ast(stdout, &albedo_ctx.ast, &albedo_ctx.tokens);
    #endif /* ifdef DEBUG_MODE */

compiler_exit:
    if (albedo_ctx.error_count > 0) {
        diagnostics_print();
        buffer_cleanup();

        fprintf(
            stderr,
            "compilation %s%sfailed%s due to %s%s%u%s errors\n",
            ANSI_BOLD,
            ANSI_RED,
            ANSI_RESET,
            ANSI_BOLD,
            ANSI_RED,
            albedo_ctx.error_count,
            ANSI_RESET
        );

        return 1;
    }

    fprintf(
        stderr,
        "\ncompiled %s%ssuccessfully%s\n",
        ANSI_BOLD,
        ANSI_MAGENTA,
        ANSI_RESET
    );

    buffer_cleanup();

    return 0;
}
