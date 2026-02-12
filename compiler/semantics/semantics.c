#include "semantics.h"

#include "../albedo/types.h"
#include "../ast/types.h"

extern AlbedoCtx albedo_ctx;

void declare_symbols() {
    for (usize i = 0; i < albedo_ctx.ast.count; i++) {
        AstNode* decl = albedo_ctx.ast.nodes[i];

        switch (decl -> kind) {
            case A_Function:
                break;

            default:
                break;
        }
    }
}

void resolve_symbols() {

}
