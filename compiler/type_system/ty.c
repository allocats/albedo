#include "ty.h"

#include "../albedo/types.h"
#include "../hash/hash.h"

#include <stdint.h>
#include <string.h>

extern AlbedoCtx albedo_ctx;

void type_table_grow(void);

void init_type_table(void) {
    TypeTable* table = &albedo_ctx.type_table;

    table -> entries = arena_alloc(albedo_ctx.arena, sizeof(TypeEntry*) * INIT_TABLE_ENTRY_CAPACITY);
    table -> entry_count = 0;
    table -> entry_capacity = INIT_TABLE_ENTRY_CAPACITY;

    arena_memset(table -> entries, 0, sizeof(TypeEntry*) * INIT_TABLE_ENTRY_CAPACITY);
}

TypeEntry* type_table_lookup(u32 hash, char* ptr, usize len) {
    TypeTable* table = &albedo_ctx.type_table;

    u32 index = hash & (table -> entry_capacity - 1);

    TypeEntry* entry = table -> entries[index];

    while (entry) {
        if (entry -> len == len && strncmp(entry -> ptr, ptr, len) == 0) {
            return entry;
        }

        entry = entry -> next;
    }

    return null;
}

void type_table_insert(TypeEntry* entry) {
    TypeTable* table = &albedo_ctx.type_table;

    if (table -> entry_count >= ((table -> entry_capacity * 3) / 4)) {
        type_table_grow();
    }

    u32 index = entry -> hash & (table -> entry_capacity - 1);

    TypeEntry* slot = table -> entries[index];

    while (slot) {
        if (slot -> len == entry -> len && strncmp(slot -> ptr, entry -> ptr, entry -> len) == 0) {
            return;
        }

        slot = slot -> next;
    }

    entry -> next = table -> entries[index];

    table -> entries[index] = entry;
    table -> entry_count += 1;
}

void type_table_grow(void) {
    TypeTable* table = &albedo_ctx.type_table;

    u32 old_capacity = table -> entry_capacity;
    u32 new_capacity = old_capacity * 2;

    TypeEntry** old_entries = table -> entries;
    TypeEntry** new_entries = arena_alloc(albedo_ctx.arena, sizeof(TypeEntry*) * new_capacity);

    for (u32 i = 0; i < old_capacity; i++) {
        TypeEntry* current = old_entries[i];

        while (current) {
            TypeEntry* next = current -> next;

            u32 new_index = current -> hash & (new_capacity - 1);

            current -> next = new_entries[new_index];
            new_entries[new_index] = current;

            current = next;
        }
    }

    table -> entries = new_entries;
    table -> entry_capacity = new_capacity;
}

TypeEntry* register_type(TypeKind kind, char* ptr, usize len, u32 size, u16 alignment) {
    TypeEntry* entry = arena_alloc(albedo_ctx.arena, sizeof(*entry));

    entry -> kind = kind;
    entry -> resolved = true;
    
    entry -> size = size;
    entry -> alignment = alignment;

    entry -> hash = fnv1a_hash(ptr, len);
    entry -> ptr = ptr;
    entry -> len = len;

    entry -> next = null;

    type_table_insert(entry);
    
    return entry;
}

void register_builtin_types(void) {
    albedo_ctx.builtin_types.void_ty = register_builtin_type("void", 0, 1);
    albedo_ctx.builtin_types.bool_ty = register_builtin_type("bool", 1, 1);

    albedo_ctx.builtin_types.u8_ty  = register_builtin_type("u8",  1, 1);
    albedo_ctx.builtin_types.u16_ty = register_builtin_type("u16", 2, 2);
    albedo_ctx.builtin_types.u32_ty = register_builtin_type("u32", 4, 4);
    albedo_ctx.builtin_types.u64_ty = register_builtin_type("u64", 8, 8);

    albedo_ctx.builtin_types.i8_ty  = register_builtin_type("i8",  1, 1);
    albedo_ctx.builtin_types.i16_ty = register_builtin_type("i16", 2, 2);
    albedo_ctx.builtin_types.i32_ty = register_builtin_type("i32", 4, 4);
    albedo_ctx.builtin_types.i64_ty = register_builtin_type("i64", 8, 8);

    albedo_ctx.builtin_types.f32_ty = register_builtin_type("f32", 4, 4);
    albedo_ctx.builtin_types.f64_ty = register_builtin_type("f64", 8, 8);

    usize pointer_size = sizeof(void*);

    albedo_ctx.builtin_types.usize_ty = register_builtin_type("usize", pointer_size, pointer_size);
    albedo_ctx.builtin_types.isize_ty = register_builtin_type("isize", pointer_size, pointer_size);
}
