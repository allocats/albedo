#pragma once
#ifndef ALBEDO_TYPES_H
#define ALBEDO_TYPES_H

#include "../arena/arena.h"
#include "../ast_parser/types.h"
#include "../buffers/types.h"
#include "../modules/types.h"
#include "../symbols/types.h"
#include "../token/types.h"

#define FILE_INIT_CAPACITY 16

typedef struct {
    ArenaAllocator* arena;

    FileBuffer* files;
    u32 file_count;
    u32 file_capacity;

    Tokens tokens;
    ParseTree ast;

    SymbolTable global_scope;

    u32 error_count;

    char* output_path;
} AlbedoCtx;

#endif // !ALBEDO_TYPES_H
