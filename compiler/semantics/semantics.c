#include "semantics.h"

#include "../albedo/types.h"
#include "../ast/types.h"
#include "../symbols/types.h"

extern AlbedoCtx albedo_ctx;

void declare_symbols() {
    for (usize i = 0; i < albedo_ctx.ast.count; i++) {
        AstNode* decl = albedo_ctx.ast.nodes[i];
        Symbol* symbol = arena_alloc(albedo_ctx.arena, sizeof(*symbol));

        symbol -> next = null;
        symbol -> ast_node = decl;

        switch (decl -> kind) {
            case A_Function:
                symbol -> kind = S_Function;
                symbol -> hash;

            default:
                asm("int3");
        }
    }
}

void resolve_symbols() {

}
