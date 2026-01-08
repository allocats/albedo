#include "diagnostics.h"

#include "../albedo/types.h"
#include "../utils/ansi_codes.h"
#include "types.h"

#include <stdio.h>

extern AlbedoCtx albedo_ctx;
extern DiagnosticCtx diag_ctx;

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
}
