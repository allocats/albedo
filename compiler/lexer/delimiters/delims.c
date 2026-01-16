#include "types.h"

#include "../../diagnostics/diagnostics.h"

#include <assert.h>
#include <stdio.h>

void delim_stack_push(DelimStack* stack, Token* token, u32 index) {
    if (stack -> top + 1 >= MAX_DELIM_STACK) {
        err_delim_stack_max(token, index);
        return;
    }

    stack -> delims[++stack -> top] = token;
}

Token* delim_stack_pop(DelimStack* stack) {
    if (stack -> top < 0) return nullptr;
    return stack -> delims[stack -> top--];
}

void match_delim(DelimStack* stack, Token* token, u32 index) {
    Token* popped = delim_stack_pop(stack);

    if (popped == nullptr) {
        err_delim_unopened(token, index);
        return;
    }

    switch (token -> kind) {
        case T_RParen: {
            if (popped -> kind != T_LParen) {
                err_delim_mismatch(token, index);
            }
        } break;

        case T_RBrace: {
            if (popped -> kind != T_LBrace) {
                err_delim_mismatch(token, index);
            }
        } break;

        case T_RBracket: {
            if (popped -> kind != T_LBracket) {
                err_delim_mismatch(token, index);
            }
        } break;

        default: {
            fprintf(stderr, "How are we here, hit default case in match_delim(). Please report\n");
            exit(1);
        } break;
    }
}
