#pragma once
#ifndef ALBEDO_BUFFERS_TYPES_H
#define ALBEDO_BUFFERS_TYPES_H

#include "../utils/types.h"

typedef struct {
    const char* path;
    char* buffer;
    usize size;
    bool needs_free;
} FileBuffer;

#endif // !ALBEDO_BUFFERS_TYPES_H
