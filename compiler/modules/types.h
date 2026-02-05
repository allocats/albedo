#pragma once
#ifndef ALBEDO_MODULES_TYPES_H
#define ALBEDO_MODULES_TYPES_H

#include "../utils/types.h"

typedef struct {
    u32 hash;
    bool imported;
    char* path;
} Module;

typedef struct {
    Module* items;
    usize count;
    usize capacity;
} Modules;

#endif // !ALBEDO_MODULES_TYPES_H
