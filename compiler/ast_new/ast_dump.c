#include "ast.h"

#include <stdio.h>

typedef struct {
    bool* has_more;  // Array indicating if each level has more siblings
    u32 depth;       // Current depth
    u32 capacity;    // Capacity of has_more array
} PrintContext;

void print_node_tree(FILE* fd, AstNode* node, PrintContext* ctx, bool is_last);
void print_tree_indent(FILE* fd, PrintContext* ctx, bool is_last, bool is_continuation);

PrintContext create_print_context(void) {
    PrintContext ctx;
    ctx.capacity = 64;
    ctx.depth = 0;
    ctx.has_more = (bool*)calloc(ctx.capacity, sizeof(bool));
    return ctx;
}

void free_print_context(PrintContext* ctx) {
    free(ctx->has_more);
}

void ensure_context_capacity(PrintContext* ctx) {
    if (ctx->depth >= ctx->capacity) {
        ctx->capacity *= 2;
        ctx->has_more = (bool*)realloc(ctx->has_more, ctx->capacity * sizeof(bool));
    }
}

void dump_ast(FILE* fd, AST* ast) {
    fprintf(fd, "============ AST DUMP =============\n\n");

    PrintContext ctx = create_print_context();

    for (usize i = 0; i < ast->count; i++) {
        bool is_last = (i == ast->count - 1);
        print_node_tree(fd, ast->nodes[i], &ctx, is_last);

        if (i != ast->count - 1) {
            fprintf(fd, "\n");
        }
    }

    fprintf(fd, "\n===================================\n\n");
    free_print_context(&ctx);
}

void print_tree_indent(FILE* fd, PrintContext* ctx, bool is_last, bool is_continuation) {
    for (u32 i = 0; i < ctx->depth; i++) {
        if (i == ctx->depth - 1 && !is_continuation) {
            if (is_last) {
                fprintf(fd, "└─ ");
            } else {
                fprintf(fd, "├─ ");
            }
        } else {
            if (ctx->has_more[i]) {
                fprintf(fd, "│  ");
            } else {
                fprintf(fd, "   ");
            }
        }
    }
}

void print_node_tree(FILE* fd, AstNode* node, PrintContext* ctx, bool is_last) {
    if (node == NULL) {
        print_tree_indent(fd, ctx, is_last, false);
        fprintf(fd, "<None>\n");
        return;
    }

    ensure_context_capacity(ctx);
    if (ctx->depth > 0) {
        ctx->has_more[ctx->depth - 1] = !is_last;
    }

    switch (node->kind) {
        case A_ParseError: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "ParseError :: [%u .. %u]\n", node->span.start, node->span.end);
            break;
        }

        case A_Import: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Import :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "\"%.*s\"\n", (i32)node->import_decl.len, node->import_decl.ptr);

            ctx->depth--;
            break;
        }

        case A_Function: {
            print_tree_indent(fd, ctx, is_last, false);
            if (node->flags & IS_EXTERNAL) {
                fprintf(fd, "External ");
            }
            fprintf(fd, "Function :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->function_decl.len, node->function_decl.ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Return Type Span :: [%u .. %u]\n",
                    node->function_decl.return_type_token_span.start,
                    node->function_decl.return_type_token_span.end);

            bool has_body = !(node->flags & IS_EXTERNAL);

            ctx->has_more[ctx->depth - 1] = has_body;
            print_tree_indent(fd, ctx, !has_body, false);
            fprintf(fd, "Parameters [%u/%u]\n", node->function_decl.param_count, node->function_decl.param_capacity);

            if (node->function_decl.param_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->function_decl.param_count; i++) {
                    AstNode* param = &node->function_decl.params[i];
                    bool is_last_param = (i == node->function_decl.param_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_param;

                    print_tree_indent(fd, ctx, is_last_param, false);
                    fprintf(fd, "%u. %.*s :: [%u .. %u]\n",
                            i,
                            (i32)param->parameter.len, param->parameter.ptr,
                            param->parameter.type_token_span.start,
                            param->parameter.type_token_span.end);
                }
                ctx->depth--;
            }

            if (has_body) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Body:\n");
                ctx->depth++;
                print_node_tree(fd, node->function_decl.body, ctx, true);
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Struct: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Struct :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->struct_decl.len, node->struct_decl.ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Fields [%u/%u]\n", node->struct_decl.field_count, node->struct_decl.field_capacity);

            if (node->struct_decl.field_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->struct_decl.field_count; i++) {
                    // Fields are stored as AstNode* with kind A_VarDecl-like structure
                    // Using AstField layout via the nodes array
                    AstNode* field_node = &node->struct_decl.fields[i];
                    bool is_last_field = (i == node->struct_decl.field_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_field;

                    print_tree_indent(fd, ctx, is_last_field, false);
                    // AstField has ptr/len and type_token_type (Span)
                    // We access via the node's union — struct fields are AstNode[] so cast
                    // NOTE: struct_decl.fields is AstNode*, each node should be inspected directly
                    print_node_tree(fd, field_node, ctx, is_last_field);
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Enum: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Enum :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->enum_decl.len, node->enum_decl.ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Underlying Type Span :: [%u .. %u]\n",
                    node->enum_decl.type_token_span.start,
                    node->enum_decl.type_token_span.end);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Variants [%u/%u]\n", node->enum_decl.variant_count, node->enum_decl.variant_capacity);

            if (node->enum_decl.variant_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->enum_decl.variant_count; i++) {
                    AstNode* variant_node = &node->enum_decl.variants[i];
                    bool is_last_variant = (i == node->enum_decl.variant_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_variant;

                    print_tree_indent(fd, ctx, is_last_variant, false);
                    fprintf(fd, "%u. \"%.*s\"", i,
                            (i32)variant_node->enum_variant.len,
                            variant_node->enum_variant.ptr);

                    if (variant_node->enum_variant.value != NULL) {
                        fprintf(fd, "\n");
                        ctx->depth++;
                        ensure_context_capacity(ctx);
                        ctx->has_more[ctx->depth - 1] = false;
                        print_tree_indent(fd, ctx, true, false);
                        fprintf(fd, "Value:\n");
                        ctx->depth++;
                        print_node_tree(fd, variant_node->enum_variant.value, ctx, true);
                        ctx->depth--;
                        ctx->depth--;
                    } else {
                        fprintf(fd, "\n");
                    }
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Union: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Union :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->union_decl.len, node->union_decl.ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Fields [%u/%u]\n", node->union_decl.field_count, node->union_decl.field_capacity);

            if (node->union_decl.field_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->union_decl.field_count; i++) {
                    AstNode* field_node = &node->union_decl.fields[i];
                    bool is_last_field = (i == node->union_decl.field_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_field;
                    print_node_tree(fd, field_node, ctx, is_last_field);
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Block: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Block :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;

            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Statements [%u/%u]\n", node->block.stmt_count, node->block.stmt_capacity);

            if (node->block.stmt_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->block.stmt_count; i++) {
                    print_node_tree(fd, node->block.stmts[i], ctx, i == node->block.stmt_count - 1);
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Defer: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Defer :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Statement:\n");
            ctx->depth++;
            print_node_tree(fd, node->defer_stmt.stmt, ctx, true);
            ctx->depth--;
            ctx->depth--;
            break;
        }

        case A_Return: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Return :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Expression:\n");
            ctx->depth++;
            print_node_tree(fd, node->return_stmt.expr, ctx, true);
            ctx->depth--;
            ctx->depth--;
            break;
        }

        case A_Continue: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Continue :: [%u .. %u]", node->span.start, node->span.end);

            if (node->continue_stmt.label_len > 0) {
                fprintf(fd, "\n");
                ctx->depth++;
                ensure_context_capacity(ctx);
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Label: \"%.*s\"\n",
                        (i32)node->continue_stmt.label_len,
                        node->continue_stmt.label_ptr);
                ctx->depth--;
            } else {
                fprintf(fd, "\n");
            }
            break;
        }

        case A_Break: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Break :: [%u .. %u]", node->span.start, node->span.end);

            if (node->break_stmt.label_len > 0) {
                fprintf(fd, "\n");
                ctx->depth++;
                ensure_context_capacity(ctx);
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Label: \"%.*s\"\n",
                        (i32)node->break_stmt.label_len,
                        node->break_stmt.label_ptr);
                ctx->depth--;
            } else {
                fprintf(fd, "\n");
            }
            break;
        }

        case A_VarDecl: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "VarDecl :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->var_decl.len, node->var_decl.ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Type Span :: [%u .. %u]\n",
                    node->var_decl.type_token_span.start,
                    node->var_decl.type_token_span.end);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Value:\n");
            ctx->depth++;
            print_node_tree(fd, node->var_decl.value, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_If: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "If :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = (node->if_stmt.else_block != NULL);
            print_tree_indent(fd, ctx, node->if_stmt.else_block == NULL, false);
            fprintf(fd, "Branches [%u/%u]\n", node->if_stmt.branch_count, node->if_stmt.branch_capacity);

            ctx->depth++;
            ensure_context_capacity(ctx);
            for (u32 i = 0; i < node->if_stmt.branch_count; i++) {
                AstIfBranch* branch = &node->if_stmt.branches[i];
                bool is_last_branch = (i == node->if_stmt.branch_count - 1);
                ctx->has_more[ctx->depth - 1] = !is_last_branch;

                print_tree_indent(fd, ctx, is_last_branch, false);
                fprintf(fd, "Branch %u\n", i);

                ctx->depth++;
                ensure_context_capacity(ctx);

                ctx->has_more[ctx->depth - 1] = true;
                print_tree_indent(fd, ctx, false, false);
                fprintf(fd, "Condition:\n");
                ctx->depth++;
                print_node_tree(fd, branch->condition, ctx, true);
                ctx->depth--;

                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Block:\n");
                ctx->depth++;
                print_node_tree(fd, branch->block, ctx, true);
                ctx->depth--;

                ctx->depth--;
            }
            ctx->depth--;

            if (node->if_stmt.else_block != NULL) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Else Block:\n");
                ctx->depth++;
                print_node_tree(fd, node->if_stmt.else_block, ctx, true);
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_While: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "While :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Condition:\n");
            ctx->depth++;
            print_node_tree(fd, node->while_loop.condition, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Block:\n");
            ctx->depth++;
            print_node_tree(fd, node->while_loop.block, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Loop: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Loop :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Block:\n");
            ctx->depth++;
            print_node_tree(fd, node->infinite_loop.block, ctx, true);
            ctx->depth--;
            ctx->depth--;
            break;
        }

        case A_For: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "For :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Iterator:\n");
            ctx->depth++;
            print_node_tree(fd, node->for_loop.iterator, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Condition:\n");
            ctx->depth++;
            print_node_tree(fd, node->for_loop.condition, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Step:\n");
            ctx->depth++;
            print_node_tree(fd, node->for_loop.step, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Block:\n");
            ctx->depth++;
            print_node_tree(fd, node->for_loop.block, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Binary: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Binary :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Operator: %s\n", TOKEN_KIND_STRINGS[node->binary_op.op]);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Left:\n");
            ctx->depth++;
            print_node_tree(fd, node->binary_op.left, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Right:\n");
            ctx->depth++;
            print_node_tree(fd, node->binary_op.right, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Unary: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Unary :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Operator: %s\n", TOKEN_KIND_STRINGS[node->unary_op.op]);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Operand:\n");
            ctx->depth++;
            print_node_tree(fd, node->unary_op.operand, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Assign: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Assign :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Operator: %s\n", TOKEN_KIND_STRINGS[node->assign.op]);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->assign.target, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Value:\n");
            ctx->depth++;
            print_node_tree(fd, node->assign.value, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Call: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Call :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->call.len, node->call.ptr);

            if (node->call.arg_count > 0) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Arguments [%u/%u]\n", node->call.arg_count, node->call.arg_capacity);

                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->call.arg_count; i++) {
                    print_node_tree(fd, node->call.args[i], ctx, i == node->call.arg_count - 1);
                }
                ctx->depth--;
            } else {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Arguments [0/0]\n");
            }

            ctx->depth--;
            break;
        }

        case A_Ident: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Ident :: [%u .. %u] -> \"%.*s\"\n",
                    node->span.start, node->span.end,
                    (i32)node->identifier.len, node->identifier.ptr);
            break;
        }

        case A_Literal: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Literal :: [%u .. %u] -> %s: \"%.*s\"\n",
                    node->span.start, node->span.end,
                    TOKEN_KIND_STRINGS[node->literal.kind],
                    (i32)node->literal.len, node->literal.ptr);
            break;
        }

        case A_Index: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Index :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->index.ident, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Index:\n");
            ctx->depth++;
            print_node_tree(fd, node->index.index, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_MemberAccess: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "MemberAccess :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->member_access.target, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Field: \"%.*s\"\n",
                    (i32)node->member_access.field_len,
                    node->member_access.field_ptr);

            ctx->depth--;
            break;
        }

        case A_Cast: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Cast :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Expression:\n");
            ctx->depth++;
            print_node_tree(fd, node->cast.target, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Target Type Span :: [%u .. %u]\n",
                    node->cast.type_token_span.start,
                    node->cast.type_token_span.end);

            ctx->depth--;
            break;
        }

        case A_StructInit: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "StructInit :: [%u .. %u]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->struct_init.len, node->struct_init.ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Field Initializers [%u/%u]\n",
                    node->struct_init.field_count, node->struct_init.field_capacity);

            if (node->struct_init.field_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->struct_init.field_count; i++) {
                    AstFieldInit* field_init = &node->struct_init.fields[i];
                    bool is_last_init = (i == node->struct_init.field_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_init;

                    print_tree_indent(fd, ctx, is_last_init, false);
                    fprintf(fd, "%u. \"%.*s\"\n", i, (i32)field_init->len, field_init->ptr);

                    ctx->depth++;
                    ensure_context_capacity(ctx);
                    ctx->has_more[ctx->depth - 1] = false;
                    print_tree_indent(fd, ctx, true, false);
                    fprintf(fd, "Value:\n");
                    ctx->depth++;
                    print_node_tree(fd, field_init->value, ctx, true);
                    ctx->depth--;
                    ctx->depth--;
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        default: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "<unknown node kind: %d>\n", node->kind);
            break;
        }
    }
}
