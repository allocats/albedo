#include "token.h"

#include "../utils/macros.h"

#include <stdio.h>

void print_token(Token* token) {
    printf("Token: {\n");
    printf("  Lexeme: %.*s\n", token -> length, token -> lexeme);
    printf("  Length: %d\n", token -> length);
    printf("}\n\n");
}

void print_tokens(Tokens tokens) {
    for (u32 i = 0; i < tokens.count; i++) {
        Token tok = tokens.items[i];
        
        printf("%d: %s\n", i, TOKEN_KIND_STRINGS[tok.kind]);
        printf("  Lexeme: \'%.*s\'\n", tok.length, tok.lexeme);
        printf("  Length: %d\n\n", tok.length);
    }
}

void extend_tokens(ArenaAllocator* arena, Tokens* tokens) {
    if (UNLIKELY(tokens -> count >= tokens -> capacity)) {
        usize old_size = tokens -> capacity * sizeof(Token); 
        usize new_size = old_size * TOKENS_GROWTH_FACTOR;

        tokens -> capacity *= TOKENS_GROWTH_FACTOR;
        tokens -> items = arena_realloc(arena, tokens -> items, old_size, new_size);
    }
}
