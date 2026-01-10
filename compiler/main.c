#include <stdio.h>
#include <unistd.h>

#include "albedo/albedo.h"
#include "albedo/types.h"
#include "arena/arena.h"
#include "buffers/buffers.h"
#include "diagnostics/diagnostics.h"
#include "lexer/lexer.h"
#include "token/token.h"
#include "utils/ansi_codes.h"
#include "utils/types.h"

static ArenaAllocator arena = {0};

extern AlbedoCtx albedo_ctx;

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

    print_tokens(albedo_ctx.tokens);

    if (albedo_ctx.error_count > 0) {
        diagnostics_print();
        buffer_cleanup();
        return 1;
    }

    buffer_cleanup();

    return 0;
}
