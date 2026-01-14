#pragma once
#ifndef ALBEDO_DIAGNOSTICS_TYPES_H
#define ALBEDO_DIAGNOSTICS_TYPES_H

#include "../utils/types.h"

#define DIAG_SUPPRESS_THRESHOLD 32

#define LOC_START_OF_TOK  (1 << 0)
#define LOC_END_OF_TOK    (1 << 1)
#define LOC_WHOLE_TOK     (1 << 2)
#define LOC_WHOLE_LINE    (1 << 3)

typedef enum {
    DIAG_ERR,
    DIAG_WARN,
    DIAG_NOTE
} DiagKind;

typedef struct {
    DiagKind kind;

    u32 index;

    u32 line;
    u32 col;
    u32 len;

    const char* msg;
    const char* help;
} Diagnostic;

typedef struct {
    Diagnostic* diags;
    u32 diag_count;
    u32 diag_capacity;
} DiagnosticCtx;

#endif // !ALBEDO_DIAGNOSTICS_TYPES_H
