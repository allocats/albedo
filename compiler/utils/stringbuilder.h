#pragma once
#ifndef ALBEDO_STRINGBUILDER_H
#define ALBEDO_STRINGBUILDER_H

#include "types.h"

#include "../arena/arena.h"

#include <stdarg.h>
#include <stdio.h>

typedef struct {
    char* buffer;
    usize count;
    usize capacity;
} StringBuilder;

static void string_builder_init(ArenaAllocator* arena, StringBuilder* sb, usize size) {
    sb -> buffer = (char*) arena_alloc(arena, size);
    sb -> count = 0;
    sb -> capacity = size;
}

#define sb_append(arena, sb, str) string_builder_append(arena, sb, str, sizeof(str))

static void string_builder_append(ArenaAllocator* arena, StringBuilder* sb, char* str, usize len) {
    if (sb -> count + len >= sb -> capacity) {
        usize new_capacity = sb -> capacity;

        while (sb -> count + len >= new_capacity) new_capacity *= 2;

        sb -> buffer = (char*) arena_realloc(arena, sb -> buffer, sb -> capacity, new_capacity);
        sb -> capacity *= 2;
    }

    arena_memcpy(sb -> buffer + sb -> count, str, len);
    sb -> count += len;
}

static void string_builder_append_fmt(ArenaAllocator* arena, StringBuilder* sb, const char* fmt, ...) {
    va_list args;

    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len <= 0) return;

    char* tmp = (char*) arena_alloc(arena, len + 1);

    va_start(args, fmt);
    vsnprintf(tmp, len + 1, fmt, args);
    va_end(args);

    string_builder_append(arena, sb, tmp, (usize) len);
}

static void string_builder_reset(StringBuilder* sb) {
    sb -> count = 0;
}

static void string_builder_fprintf(FILE* fd, StringBuilder sb) {
    fprintf(fd, "%.*s", (i32)sb.count, sb.buffer);
}

#endif // !ALBEDO_STRINGBUILDER_H
