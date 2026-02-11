#include "symbols.h"

#include "../albedo/types.h"

extern AlbedoCtx albedo_ctx;

void init_symbol_table(SymbolTable* table) {
    ArenaAllocator* arena = albedo_ctx.arena;

    table -> next_scope = null;

    table -> symbols = arena_alloc(arena, sizeof(Symbol*) * INIT_SYMBOL_CAP);
    table -> count = 0;
    table -> capacity = INIT_SYMBOL_CAP;
}

Symbol* get_symbol(SymbolTable* table, u32 hash) {
    usize index = hash & (table -> capacity - 1);
    return table -> symbols[index];
}

bool add_symbol(SymbolTable* table, Symbol* symbol) {
    Symbol* slot = get_symbol(table, symbol -> hash);

    if (slot) {
        return false;
    }

    if (table -> count >= table -> capacity) {
        usize size = table -> capacity * sizeof(Symbol*);

        table -> symbols = arena_realloc(albedo_ctx.arena, table -> symbols, size, size * 2);
        table -> capacity *= 2;
    }

    table -> count++;
    slot = symbol;
    return true;
}
