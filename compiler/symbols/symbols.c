#include "symbols.h"

#include "../albedo/types.h"

extern AlbedoCtx albedo_ctx;

void init_symbol_table(SymbolTable* table) {
    ArenaAllocator* arena = albedo_ctx.arena;

    table -> next_scope = null;

    table -> symbols = arena_alloc(arena, sizeof(Symbol*) * INIT_SYMBOL_CAP);
    table -> symbol_count = 0;
    table -> symbol_capacity = INIT_SYMBOL_CAP;
}
