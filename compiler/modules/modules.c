#include "modules.h"

#include "../albedo/types.h"
#include "../ast_parser/parser.h"
#include "../buffers/buffers.h"
#include "../hash/hash.h"
#include "../lexer/lexer.h"
#include "types.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#define INIT_MODULE_CAPACITY 32
#define PATH_LEN 1024

extern AlbedoCtx albedo_ctx;
extern AstNodeList import_nodes_list;

char* HOME = null; 
char* root = null;

void load_standard_library_modules(Modules* modules);
void add_standard_library_module(Modules* modules, char* ptr, usize len);
Module* get_standard_library_module(Modules* modules, u32 hash);

void add_imported_module(Modules* modules, Module module);

void parse_module(Module* module);

void init_module_system(Modules* std_modules) {
    HOME = getenv("HOME");

    std_modules -> items = arena_alloc(albedo_ctx.arena, sizeof(Module) * INIT_MODULE_CAPACITY);
    std_modules -> count = 0;
    std_modules -> capacity = INIT_MODULE_CAPACITY;

    load_standard_library_modules(std_modules);
}

void load_standard_library_modules(Modules* modules) {
    root = arena_alloc(albedo_ctx.arena, PATH_LEN);

    snprintf(root, PATH_LEN - 1, "%s/%s", HOME, ".local/mythril");

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
                    add_standard_library_module(modules, entry_path, entry_len);
                }
            }
        }

        closedir(directory);
    }
}

void add_standard_library_module(Modules* modules, char* ptr, usize len) {
    if (modules -> count >= modules -> capacity) {
        usize size = modules -> capacity * sizeof(Module);

        modules -> items = arena_realloc(albedo_ctx.arena, modules -> items, size, size * 2); 
        modules -> capacity *= 2;
    }

    u32 hash = fnv1a_hash(ptr, len);
    
    Module module = {
        .hash = hash,
        .path = ptr,
        .imported = false
    };

    usize index = hash & (modules -> capacity - 1);

    modules -> items[index] = module;
    modules -> count++;

    #ifdef DEBUG_MODE
    printf("Loaded stdlib module: %s\n", ptr);
    #endif /* ifdef DEBUG_MODE */
}

Module* get_standard_library_module(Modules* modules, u32 hash) {
    usize index = hash & (modules -> capacity - 1); 
    Module* module = &modules -> items[index];

    if (module -> hash == 0) return null;
    return module;
}

void resolve_modules(Modules* std_modules) {
    usize processed = 0;

    while (processed < import_nodes_list.count) {
        AstNode* node = import_nodes_list.nodes[processed++];

        char path[PATH_LEN];

        char* ptr = node -> import_decl.ptr;
        usize len = node -> import_decl.len;

        char temp_char = ptr[len];
        ptr[len] = 0;


        usize n = snprintf(path, PATH_LEN - 1, "%s/%s.myth", root, ptr);
        u32 hash = fnv1a_hash(path, n);

        Module* module = get_standard_library_module(std_modules, hash);

        if (module) {
            if (!module -> imported) {
                parse_module(module);

                module -> imported = true;
            }
        } else {
            // TODO: Search relative, if not found then error
        }

        ptr[len] = temp_char;
    }
}

void parse_module(Module* module) {
    #ifdef DEBUG_MODE
    printf("Parsing module: %s\n", module -> path);
    #endif /* ifdef DEBUG_MODE */

    usize token_index = albedo_ctx.tokens.count;

    map_file(module -> path);
    lex_from_files(albedo_ctx.file_count - 1);
    parse_tokens(token_index, albedo_ctx.file_count - 1);
}
