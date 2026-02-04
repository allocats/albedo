#pragma once
#ifndef ALBEDO_MODULES_TYPES_H
#define ALBEDO_MODULES_TYPES_H

#include "../ast/types.h"
#include "../utils/types.h"

typedef struct ModuleNode {
    u32 hash;
    char* path; // null terminated, owned string
    AstNode* ast_node;

    // Chaining incase of clash
    struct ModuleNode* next;
} ModuleNode;

typedef struct {
    ModuleNode** modules;
    usize module_count;
    usize module_capacity;
} ModuleLoader;

#endif // !ALBEDO_MODULES_TYPES_H
