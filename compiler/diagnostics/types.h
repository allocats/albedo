#pragma once
#ifndef ALBEDO_DIAGNOSTICS_TYPES_H
#define ALBEDO_DIAGNOSTICS_TYPES_H

#include "../utils/types.h"

#define DIAG_SUPPRESS_THRESHOLD 32

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
