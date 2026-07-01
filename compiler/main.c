#include <stdio.h>
#include <unistd.h>

#include "albedo/albedo.h"
#include "albedo/types.h"
#include "ast/types.h"
#include "ast_parser/parser.h"
#include "arena/arena.h"
#include "buffers/buffers.h"
#include "diagnostics/diagnostics.h"
#include "lexer/lexer.h"
#include "modules/modules.h"
#include "semantics/semantics.h"
#include "symbols/symbols.h"
#include "utils/ansi_codes.h"
#include "utils/timer.h"
#include "utils/types.h"

#ifdef DEBUG_MODE
#include <assert.h>
#endif /* ifdef DEBUG_MODE */

static ArenaAllocator arena = {0};

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;
extern Modules stdlib_modules;

i32 main(i32 argc, char* argv[]) {
    #ifdef DEBUG_MODE
    static_assert(sizeof(Token) == 16, "Token is not 16 bytes");
    static_assert(sizeof(Symbol) == 24, "Symbol is not 32 bytes");

    // Need to rethink AstNode design
    static_assert(sizeof(AstNode) == 88, "AstNode is not 88 bytes");
    #endif /* ifdef DEBUG_MODE */

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
    init_module_system(&stdlib_modules);

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

    Timer timer = {0};

    timer_start(&timer);

    lex_from_files(0);

    if (albedo_ctx.error_count > 0) {
        goto compiler_exit;
    }

    parse_tokens(0, 0);

    resolve_modules(&stdlib_modules);

    declare_symbols();

    timer_end(&timer);

    #ifdef DEBUG_MODE
    #include "ast/ast.h"
    #include "token/token.h"

    print_tokens(albedo_ctx.tokens);
    print_ast(stdout, &albedo_ctx.ast, &albedo_ctx.tokens);
    print_arena_stats(&arena);
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
        "\ncompiled %s%ssuccessfully%s in %.3fms\n",
        ANSI_BOLD,
        ANSI_MAGENTA,
        ANSI_RESET,
        time_elapsed_in_ms(timer)
    );

    buffer_cleanup();

    return 0;
}
