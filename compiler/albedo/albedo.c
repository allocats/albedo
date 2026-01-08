#include "albedo.h"

#include "../buffers/buffers.h"
#include "../diagnostics/diagnostics.h"

#include <stdio.h>

AlbedoCtx albedo_ctx = {0};
DiagnosticCtx diag_ctx = {0};

void init_compiler(ArenaAllocator* arena, i32 argc, char* argv[]) {
    albedo_ctx.arena = arena;

    albedo_ctx.file_count = 0;
    albedo_ctx.file_capacity = FILE_INIT_CAPACITY;
    albedo_ctx.files = arena_alloc(arena, sizeof(FileBuffer) * FILE_INIT_CAPACITY);

    albedo_ctx.error_count = 0;
    albedo_ctx.output_path = "./a.out";

    albedo_ctx.tokens.items = arena_alloc(arena, sizeof(Token) * 64);
    albedo_ctx.tokens.capacity = 64;
    albedo_ctx.tokens.count = 0;

    diag_ctx.diag_count = 0;
    diag_ctx.diag_capacity = 32;
    diag_ctx.diags = arena_alloc(arena, sizeof(Diagnostic) * 32);

    for (i32 i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            map_file(argv[i]);
        } else {
            printf("TODO: Flag = %s\n", argv[i]);
        } 
    }
}
