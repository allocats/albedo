#include "symbols.h"

#include "../albedo/types.h"

extern AlbedoCtx albedo_ctx;

static inline u64 align_p2(u64 size) {
    return size == 1 ? 1 : 1 << (64 - __builtin_clzl(size - 1));
}

void init_symbol_table(SymbolTable* table, usize size, SymbolTable* parent_scope) {
    usize aligned_size = align_p2(size);

    table -> symbols = arena_alloc(albedo_ctx.arena, sizeof(Symbol) * size);
    table -> symbol_count = 0;
    table -> symbol_capacity = size;

    table -> slots = arena_alloc(albedo_ctx.arena, sizeof(u32) * aligned_size);
    table -> slot_capacity = aligned_size;

    table -> parent_scope = parent_scope;
}

void insert_symbol_table(SymbolTable* table, Symbol sym) {
}
