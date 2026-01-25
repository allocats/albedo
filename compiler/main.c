#include <stdio.h>
#include <unistd.h>

#include "albedo/albedo.h"
#include "albedo/types.h"
#include "ast/ast.h"
#include "ast_parser/parser.h"
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

    parse_tokens();

    #ifdef DEBUG_MODE
    print_tokens(albedo_ctx.tokens);
    print_ast(&albedo_ctx.ast);
    #endif /* ifdef DEBUG_MODE */

    if (albedo_ctx.error_count > 0) {
        diagnostics_print();
        printf("failed");
        buffer_cleanup();
        return 1;
    }

    printf("success");
    buffer_cleanup();

    return 0;
}
