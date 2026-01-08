#pragma once
#ifndef ALBEDO_DIAGNOSTICS_TYPES_H
#define ALBEDO_DIAGNOSTICS_TYPES_H

#include "../utils/types.h"

#define DIAG_MAX 32

typedef enum {
    DIAG_ERR,
    DIAG_WARN,
    DIAG_NOTE
} DiagKind;

typedef struct {
    u32 line;
    u32 col;

    u32 index;

    const char* path;
    const char* msg;
    const char* help;

    DiagKind kind;
} Diagnostic;

typedef struct {
    u32 diag_count;
    Diagnostic diags[DIAG_MAX];
} DiagnosticCtx;

#endif // !ALBEDO_DIAGNOSTICS_TYPES_H
