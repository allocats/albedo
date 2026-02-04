#include "modules.h"

#include "../albedo/types.h"
#include "../hash/hash.h"
#include "types.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define PATH_LEN 1024

extern AlbedoCtx albedo_ctx;
extern AstNodeList import_nodes_list;

void loader_add_module(char* path, usize len);
ModuleNode* loader_get_module(u32 hash);

void init_module_loader(void) {
    ModuleLoader* loader = &albedo_ctx.modules;

    loader -> modules = arena_alloc(albedo_ctx.arena, sizeof(ModuleNode*) * INIT_MODULE_CAP);
    loader -> module_count = 0;
    loader -> module_capacity = INIT_MODULE_CAP;

    char* home = getenv("HOME");
    char* root = arena_alloc(albedo_ctx.arena, PATH_LEN);

    snprintf(root, PATH_LEN - 1, "%s/%s", home, ".local/mythril");

    char* stack[256];
    usize top = 0;

    stack[top++] = root;
    
    while (top > 0) {
        char* current_dir = stack[--top];

        DIR* directory = opendir(current_dir);
        assert(directory);

        struct dirent* entry;

        while ((entry = readdir(directory)) != null) {
            if (entry -> d_name[0] == '.') continue;

            char* entry_path = arena_alloc(albedo_ctx.arena, PATH_LEN);
            usize entry_len = snprintf(
                entry_path,
                PATH_LEN - 1,
                "%s/%s",
                current_dir,
                entry -> d_name
            ); 

            if (entry -> d_type == DT_DIR) {
                stack[top++] = entry_path;
            } else if (entry -> d_type == DT_REG) {
                usize length = strlen(entry -> d_name);

                if (length > 5 && strcmp(entry -> d_name + length - 5, ".myth") == 0) {
                    loader_add_module(entry_path, entry_len);
                }
            }
        }

        closedir(directory);
    }
}

void loader_add_module(char* path, usize len) {
    u32 hash = fnv1a_hash(path, len);

    ModuleLoader* loader = &albedo_ctx.modules;

    if (loader -> module_count >= loader -> module_capacity) {
        usize size = loader -> module_capacity * sizeof(ModuleNode*);

        loader -> modules = arena_realloc(albedo_ctx.arena, loader -> modules, size, size * 2);
        loader -> module_capacity *= 2;
    }

    ModuleNode* module = loader_get_module(hash);

    if (module) return;

    module = arena_alloc(albedo_ctx.arena, sizeof(*module));
    module -> hash = hash;
    module -> path = path;

    loader -> module_count++;
    loader -> modules[hash & (loader -> module_capacity - 1)] = module;
}

ModuleNode* loader_get_module(u32 hash) {
    ModuleLoader* loader = &albedo_ctx.modules;
    usize index = hash & (loader -> module_capacity - 1);
    return loader -> modules[index];
}

void resolve_modules(void) {
    char root[PATH_LEN] = {0};

    char* home = getenv("HOME");

    snprintf(root, PATH_LEN - 1, "%s/%s", home, ".local/mythril");

    for (usize i = 0; i < import_nodes_list.count; i++) {
        AstNode* node = import_nodes_list.nodes[i];

        char* ptr = node -> import_decl.ptr;
        usize len = node -> import_decl.length;

        char temp_c = ptr[len];
        ptr[len] = 0;

        char* module_path = arena_alloc(albedo_ctx.arena, PATH_LEN);

        usize n = snprintf(module_path, PATH_LEN - 1, "%s/%s.myth", root, ptr);
        u32 hash = fnv1a_hash(module_path, n);

        if (loader_get_module(hash)) {
            printf("YAY\n");
        } else {
            printf("NO\n");
        }

        ptr[len] = temp_c;
    }
}
