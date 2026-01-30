#include "ast.h"
#include "types.h"

#include <stdio.h>

typedef struct {
    bool* has_more;  // Array indicating if each level has more siblings
    u32 depth;       // Current depth
    u32 capacity;    // Capacity of has_more array
} PrintContext;

void print_node_tree(FILE* fd, AstNode* node, Tokens* tokens, PrintContext* ctx, bool is_last);
void print_tree_indent(FILE* fd, PrintContext* ctx, bool is_last, bool is_continuation);
void print_segment(FILE* fd, AstSegment segment);
void print_span(FILE* fd, Tokens* tokens, AstSpan span);

PrintContext create_print_context(void) {
    PrintContext ctx;
    ctx.capacity = 64;
    ctx.depth = 0;
    ctx.has_more = (bool*)calloc(ctx.capacity, sizeof(bool));
    return ctx;
}

void free_print_context(PrintContext* ctx) {
    free(ctx -> has_more);
}

void ensure_context_capacity(PrintContext* ctx) {
    if (ctx->depth >= ctx->capacity) {
        ctx->capacity *= 2;
        ctx->has_more = (bool*)realloc(ctx->has_more, ctx->capacity * sizeof(bool));
    }
}

void print_ast(FILE* fd, ParseTree* tree, Tokens* tokens) {
    fprintf(fd, "============ AST DUMP =============\n\n"); 

    PrintContext ctx = create_print_context();

    for (usize i = 0; i < tree->count; i++) {
        bool is_last = (i == tree->count - 1);
        print_node_tree(fd, tree->nodes[i], tokens, &ctx, is_last);

        if (i != tree->count - 1) {
            fprintf(fd, "\n");
        }
    }

    fprintf(fd, "\n===================================\n\n"); 
    free_print_context(&ctx);
}

void print_tree_indent(FILE* fd, PrintContext* ctx, bool is_last, bool is_continuation) {
    for (u32 i = 0; i < ctx->depth; i++) {
        if (i == ctx->depth - 1 && !is_continuation) {
            // Last level - print the connector
            if (is_last) {
                fprintf(fd, "└─ ");
            } else {
                fprintf(fd, "├─ ");
            }
        } else {
            // Earlier levels - print vertical bar if there are more siblings
            if (ctx->has_more[i]) {
                fprintf(fd, "│  ");
            } else {
                fprintf(fd, "   ");
            }
        }
    }
}

void print_continuation_indent(FILE* fd, PrintContext* ctx) {
    for (u32 i = 0; i < ctx->depth; i++) {
        if (ctx->has_more[i]) {
            fprintf(fd, "│  ");
        } else {
            fprintf(fd, "   ");
        }
    }
}

void print_node_tree(FILE* fd, AstNode* node, Tokens* tokens, PrintContext* ctx, bool is_last) {
    if (node == nullptr) {
        print_tree_indent(fd, ctx, is_last, false);
        fprintf(fd, "<None>\n");
        return;
    }

    // Update context
    ensure_context_capacity(ctx);
    if (ctx->depth > 0) {
        ctx->has_more[ctx->depth - 1] = !is_last;
    }

    switch (node->kind) {
        case A_ParseError:
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "ParseError :: [%zu .. %zu]\n", node->span.start, node->span.end);
            break;

        case A_Module: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Module :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = true;

            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Segments [%u/%u]\n", node->module_decl.segment_count, node->module_decl.segment_capacity);

            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Path: ");
            for (u32 i = 0; i < node->module_decl.segment_count; i++) {
                print_segment(fd, node->module_decl.segments[i]);
                if (i != node->module_decl.segment_count - 1) {
                    fprintf(fd, "::");
                }
            }
            fprintf(fd, "\n");

            ctx->depth--;
            break;
        }

        case A_Import: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Import :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            bool is_rel = node->import_decl.kind == ImportRel;
            ctx->has_more[ctx->depth - 1] = true;

            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Kind: %s\n", is_rel ? "Relative" : "Library");

            print_tree_indent(fd, ctx, true, false);
            if (is_rel) {
                fprintf(fd, "Path: \"%.*s\"\n", (i32)node->import_decl.relative.len, node->import_decl.relative.ptr);
            } else {
                fprintf(fd, "Path: ");
                for (u32 i = 0; i < node->import_decl.lib.segment_count; i++) {
                    print_segment(fd, node->import_decl.lib.segments[i]);
                    if (i != node->import_decl.lib.segment_count - 1) {
                        fprintf(fd, "::");
                    }
                }
                fprintf(fd, "\n");
            }

            ctx->depth--;
            break;
        }

        case A_ExternFn: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "ExternFn :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            u32 item_count = 4 + (node->extern_fn.is_variadic ? 1 : 0);
            u32 current_item = 0;

            ctx->has_more[ctx->depth - 1] = (current_item < item_count - 1);
            print_tree_indent(fd, ctx, current_item == item_count - 1, false);
            fprintf(fd, "ABI: \"%.*s\"\n", (i32)node->extern_fn.abi_len, node->extern_fn.abi_ptr);
            current_item++;

            ctx->has_more[ctx->depth - 1] = (current_item < item_count - 1);
            print_tree_indent(fd, ctx, current_item == item_count - 1, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->extern_fn.name_len, node->extern_fn.name_ptr);
            current_item++;

            ctx->has_more[ctx->depth - 1] = (current_item < item_count - 1);
            print_tree_indent(fd, ctx, current_item == item_count - 1, false);
            fprintf(fd, "Return Type Span :: [%u .. %u] -> ", 
                    node->extern_fn.return_type.start_index, node->extern_fn.return_type.end_index);
            if (node->extern_fn.return_type.start_index != 0) {
                print_span(fd, tokens, node->extern_fn.return_type);
            } else {
                fprintf(fd, "void");
            }
            fprintf(fd, "\n");
            current_item++;

            ctx->has_more[ctx->depth - 1] = (current_item < item_count - 1);
            print_tree_indent(fd, ctx, current_item == item_count - 1, false);
            fprintf(fd, "Parameters [%u/%u]\n", node->extern_fn.param_count, node->extern_fn.param_capacity);
            
            if (node->extern_fn.param_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->extern_fn.param_count; i++) {
                    ctx->has_more[ctx->depth - 1] = (i < node->extern_fn.param_count - 1);
                    print_tree_indent(fd, ctx, i == node->extern_fn.param_count - 1, false);
                    fprintf(fd, "%u. %.*s: ", i, (i32)node->extern_fn.params[i].name_len, node->extern_fn.params[i].name_ptr);
                    print_span(fd, tokens, node->extern_fn.params[i].type);
                    fprintf(fd, "\n");
                }
                ctx->depth--;
            }
            current_item++;

            if (node->extern_fn.is_variadic) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Variadic: true\n");
            }

            ctx->depth--;
            break;
        }

        case A_ExternStruct: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "ExternStruct :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "ABI: \"%.*s\"\n", (i32)node->extern_struct.abi_len, node->extern_struct.abi_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->extern_struct.name_len, node->extern_struct.name_ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Fields [%u/%u]\n", node->extern_struct.field_count, node->extern_struct.field_capacity);

            if (node->extern_struct.field_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->extern_struct.field_count; i++) {
                    AstField* field = &node->extern_struct.fields[i];
                    bool is_last_field = (i == node->extern_struct.field_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_field;

                    print_tree_indent(fd, ctx, is_last_field, false);
                    fprintf(fd, "%u. %.*s", i, (i32)field->name_len, field->name_ptr);

                    fprintf(fd, ": ");
                    print_span(fd, tokens, field->type_span);
                    fprintf(fd, "\n");

                    // if (field->kind == FieldBasic) {
                    //     fprintf(fd, ": ");
                    //     print_span(fd, tokens, field->basic_type);
                    //     fprintf(fd, "\n");
                    // } else {
                    //     fprintf(fd, "\n");
                    //     ctx->depth++;
                    //     ensure_context_capacity(ctx);
                    //     ctx->has_more[ctx->depth - 1] = (field->default_value != nullptr);
                    //
                    //     if (field->kind == FieldStruct) {
                    //         print_node_tree(fd, field->struct_decl, tokens, ctx, field->default_value == nullptr);
                    //     } else if (field->kind == FieldUnion) {
                    //         print_node_tree(fd, field->union_decl, tokens, ctx, field->default_value == nullptr);
                    //     }
                    //
                    //     if (field->default_value != nullptr) {
                    //         ctx->has_more[ctx->depth - 1] = false;
                    //         print_tree_indent(fd, ctx, true, false);
                    //         fprintf(fd, "Default Value:\n");
                    //         ctx->depth++;
                    //         print_node_tree(fd, field->default_value, tokens, ctx, true);
                    //         ctx->depth--;
                    //     }
                    //     ctx->depth--;
                    // }
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_ExternType: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "ExternType :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "ABI: \"%.*s\"\n", (i32)node->extern_type.abi_len, node->extern_type.abi_ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->extern_type.name_len, node->extern_type.name_ptr);

            ctx->depth--;
            break;
        }

        case A_Function: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Function :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(
                fd,
                "Name: \"%.*s\"\n",
                (i32) node->function_decl.ident->ident.len,
                node->function_decl.ident->ident.ptr
            );

            if (node->function_decl.ident->ident.namespace_count > 0) {
                ctx->has_more[ctx->depth - 1] = true;
                print_tree_indent(fd, ctx, false, false);
                fprintf(
                    fd,
                    "Namespace: "
                );

                for (u32 i = 0; i < node->function_decl.ident->ident.namespace_count; i++) {
                    fprintf(
                        fd,
                        "%.*s",
                        (i32) node->function_decl.ident->ident.namespaces[i].len,
                        node->function_decl.ident->ident.namespaces[i].ptr
                    );

                    if (i != node->function_decl.ident->ident.namespace_count - 1) {
                        fprintf(fd, "::");
                    }
                }

                fprintf(fd, "\n");
            }


            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Return Type Span :: [%u .. %u] -> ", 
                    node->function_decl.return_type.start_index, node->function_decl.return_type.end_index);
            if (node->function_decl.return_type.start_index != 0) {
                print_span(fd, tokens, node->function_decl.return_type);
            } else {
                fprintf(fd, "void");
            }
            fprintf(fd, "\n");

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Generics [%u/%u]\n", node->function_decl.generic_count, node->function_decl.generic_capacity);

            if (node->function_decl.generic_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->function_decl.generic_count; i++) {
                    ctx->has_more[ctx->depth - 1] = (i < node->function_decl.generic_count - 1);
                    print_tree_indent(fd, ctx, i == node->function_decl.generic_count - 1, false);
                    fprintf(fd, "%u. %.*s\n", i, (i32)node->function_decl.generics[i].name_len, 
                            node->function_decl.generics[i].name_ptr);
                }
                ctx->depth--;
            }

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Parameters [%u/%u]\n", node->function_decl.param_count, node->function_decl.param_capacity);

            if (node->function_decl.param_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->function_decl.param_count; i++) {
                    ctx->has_more[ctx->depth - 1] = (i < node->function_decl.param_count - 1);
                    print_tree_indent(fd, ctx, i == node->function_decl.param_count - 1, false);
                    fprintf(fd, "%u. %.*s: ", i, (i32)node->function_decl.params[i].name_len, 
                            node->function_decl.params[i].name_ptr);
                    print_span(fd, tokens, node->function_decl.params[i].type);
                    fprintf(fd, "\n");
                }
                ctx->depth--;
            }

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Body:\n");
            ctx->depth++;
            print_node_tree(fd, node->function_decl.body, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Struct: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Struct :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->struct_decl.name_len, node->struct_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Generics [%u/%u]\n", node->struct_decl.generic_count, node->struct_decl.generic_capacity);

            if (node->struct_decl.generic_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->struct_decl.generic_count; i++) {
                    ctx->has_more[ctx->depth - 1] = (i < node->struct_decl.generic_count - 1);
                    print_tree_indent(fd, ctx, i == node->struct_decl.generic_count - 1, false);
                    fprintf(fd, "%u. %.*s\n", i, (i32)node->struct_decl.generics[i].name_len, 
                            node->struct_decl.generics[i].name_ptr);
                }
                ctx->depth--;
            }

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Fields [%u/%u]\n", node->struct_decl.field_count, node->struct_decl.field_capacity);

            if (node->struct_decl.field_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->struct_decl.field_count; i++) {
                    AstField* field = &node->struct_decl.fields[i];
                    bool is_last_field = (i == node->struct_decl.field_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_field;

                    // const char* kind_str = field->kind == FieldBasic ? "Basic" : 
                    //                       (field->kind == FieldStruct ? "Struct" : "Union");

                    print_tree_indent(fd, ctx, is_last_field, false);
                    fprintf(fd, "%u. %.*s", i, (i32)field->name_len, field->name_ptr);

                    fprintf(fd, ": ");
                    print_span(fd, tokens, field->type_span);
                    fprintf(fd, "\n");

                    // if (field->kind == FieldBasic) {
                    //     fprintf(fd, ": ");
                    //     print_span(fd, tokens, field->basic_type);
                    //     fprintf(fd, "\n");
                    //
                    //     if (field->default_value != nullptr) {
                    //         ctx->depth++;
                    //         ensure_context_capacity(ctx);
                    //         ctx->has_more[ctx->depth - 1] = false;
                    //         print_tree_indent(fd, ctx, true, false);
                    //         fprintf(fd, "Default Value:\n");
                    //         ctx->depth++;
                    //         print_node_tree(fd, field->default_value, tokens, ctx, true);
                    //         ctx->depth--;
                    //         ctx->depth--;
                    //     }
                    // } else {
                    //     fprintf(fd, "\n");
                    //     ctx->depth++;
                    //     ensure_context_capacity(ctx);
                    //     ctx->has_more[ctx->depth - 1] = (field->default_value != nullptr);
                    //
                    //     if (field->kind == FieldStruct) {
                    //         print_node_tree(fd, field->struct_decl, tokens, ctx, field->default_value == nullptr);
                    //     } else if (field->kind == FieldUnion) {
                    //         print_node_tree(fd, field->union_decl, tokens, ctx, field->default_value == nullptr);
                    //     }
                    //
                    //     if (field->default_value != nullptr) {
                    //         ctx->has_more[ctx->depth - 1] = false;
                    //         print_tree_indent(fd, ctx, true, false);
                    //         fprintf(fd, "Default Value:\n");
                    //         ctx->depth++;
                    //         print_node_tree(fd, field->default_value, tokens, ctx, true);
                    //         ctx->depth--;
                    //     }
                    //     ctx->depth--;
                    // }
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Enum: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Enum :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->enum_decl.name_len, node->enum_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Underlying Type Span :: [%u .. %u] -> ", 
                    node->enum_decl.type.start_index, node->enum_decl.type.end_index);
            if (node->enum_decl.type.start_index != 0) {
                print_span(fd, tokens, node->enum_decl.type);
            } else {
                fprintf(fd, "i32 (default)");
            }
            fprintf(fd, "\n");

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Variants [%u/%u]\n", node->enum_decl.variant_count, node->enum_decl.variant_capacity);

            if (node->enum_decl.variant_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->enum_decl.variant_count; i++) {
                    AstEnumVariant* variant = &node->enum_decl.variants[i];
                    bool is_last_variant = (i == node->enum_decl.variant_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_variant;

                    print_tree_indent(fd, ctx, is_last_variant, false);
                    fprintf(fd, "%u. %.*s", i, (i32)variant->name_len, variant->name_ptr);

                    if (variant->value != nullptr) {
                        fprintf(fd, "\n");
                        ctx->depth++;
                        ensure_context_capacity(ctx);
                        ctx->has_more[ctx->depth - 1] = false;
                        print_tree_indent(fd, ctx, true, false);
                        fprintf(fd, "Value:\n");
                        ctx->depth++;
                        print_node_tree(fd, variant->value, tokens, ctx, true);
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

        case A_Tunion: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Tunion :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->tunion_decl.name_len, node->tunion_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Variants [%u/%u]\n", node->tunion_decl.variant_count, node->tunion_decl.variant_capacity);

            if (node->tunion_decl.variant_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->tunion_decl.variant_count; i++) {
                    AstTunionVariant* variant = &node->tunion_decl.variants[i];
                    bool is_last_variant = (i == node->tunion_decl.variant_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_variant;

                    const char* kind_str = variant->kind == VariantUnit ? "Unit" :
                                          (variant->kind == VariantTuple ? "Tuple" :
                                          (variant->kind == VariantStruct ? "Struct" : "Union"));

                    print_tree_indent(fd, ctx, is_last_variant, false);
                    fprintf(fd, "%u. [%s] %.*s\n", i, kind_str, (i32)variant->name_len, variant->name_ptr);

                    if (variant->kind == VariantTuple) {
                        ctx->depth++;
                        ensure_context_capacity(ctx);
                        ctx->has_more[ctx->depth - 1] = false;
                        print_tree_indent(fd, ctx, true, false);
                        fprintf(fd, "Types [%u/%u]\n", variant->tuple.type_count, variant->tuple.type_capacity);

                        if (variant->tuple.type_count > 0) {
                            ctx->depth++;
                            ensure_context_capacity(ctx);
                            for (u32 j = 0; j < variant->tuple.type_count; j++) {
                                ctx->has_more[ctx->depth - 1] = (j < variant->tuple.type_count - 1);
                                print_tree_indent(fd, ctx, j == variant->tuple.type_count - 1, false);
                                fprintf(fd, "%u. ", j);
                                print_span(fd, tokens, variant->tuple.types[j]);
                                fprintf(fd, "\n");
                            }
                            ctx->depth--;
                        }
                        ctx->depth--;
                    } else if (variant->kind == VariantStruct) {
                        ctx->depth++;
                        ensure_context_capacity(ctx);
                        ctx->has_more[ctx->depth - 1] = false;
                        print_tree_indent(fd, ctx, true, false);
                        fprintf(fd, "Fields [%u/%u]\n", variant->struct_fields.field_count, 
                                variant->struct_fields.field_capacity);

                        if (variant->struct_fields.field_count > 0) {
                            ctx->depth++;
                            ensure_context_capacity(ctx);
                            for (u32 j = 0; j < variant->struct_fields.field_count; j++) {
                                AstField* field = &variant->struct_fields.fields[j];
                                bool is_last_field = (j == variant->struct_fields.field_count - 1);
                                ctx->has_more[ctx->depth - 1] = !is_last_field;

                                print_tree_indent(fd, ctx, is_last_field, false);
                                fprintf(fd, "%u. %.*s", j, (i32)field->name_len, field->name_ptr);

                                    fprintf(fd, ": ");
                                    print_span(fd, tokens, field->type_span);
                                    fprintf(fd, "\n");

                                // if (field->kind == FieldBasic) {
                                //     fprintf(fd, ": ");
                                //     print_span(fd, tokens, field->basic_type);
                                //     fprintf(fd, "\n");
                                // } else {
                                //     fprintf(fd, "\n");
                                //     ctx->depth++;
                                //     if (field->kind == FieldStruct) {
                                //         print_node_tree(fd, field->struct_decl, tokens, ctx, true);
                                //     } else if (field->kind == FieldUnion) {
                                //         print_node_tree(fd, field->union_decl, tokens, ctx, true);
                                //     }
                                //     ctx->depth--;
                                // }
                            }
                            ctx->depth--;
                        }
                        ctx->depth--;
                    }
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Union: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Union :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->union_decl.name_len, node->union_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Fields [%u/%u]\n", node->union_decl.field_count, node->union_decl.field_capacity);

            if (node->union_decl.field_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->union_decl.field_count; i++) {
                    AstField* field = &node->union_decl.fields[i];
                    bool is_last_field = (i == node->union_decl.field_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_field;

                    // const char* kind_str = field->kind == FieldBasic ? "Basic" : 
                    //                       (field->kind == FieldStruct ? "Struct" : "Union");

                    print_tree_indent(fd, ctx, is_last_field, false);
                    // fprintf(fd, "%u. [%s] %.*s", i, kind_str, (i32)field->name_len, field->name_ptr);
                    fprintf(fd, "%u. %.*s", i, (i32)field->name_len, field->name_ptr);

                    fprintf(fd, ": ");
                    print_span(fd, tokens, field->type_span);
                    fprintf(fd, "\n");

                    // if (field->kind == FieldBasic) {
                    //     fprintf(fd, ": ");
                    //     print_span(fd, tokens, field->basic_type);
                    //     fprintf(fd, "\n");
                    // } else {
                    //     fprintf(fd, "\n");
                    //     ctx->depth++;
                    //     if (field->kind == FieldStruct) {
                    //         print_node_tree(fd, field->struct_decl, tokens, ctx, true);
                    //     } else if (field->kind == FieldUnion) {
                    //         print_node_tree(fd, field->union_decl, tokens, ctx, true);
                    //     }
                    //     ctx->depth--;
                    // }
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Block: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Block :: [%zu .. %zu]\n", node->span.start, node->span.end);

            if (node->block.stmt_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Statements [%zu/%zu]\n", node->block.stmt_count, node->block.stmt_capacity);

                ctx->depth++;
                ensure_context_capacity(ctx);
                for (usize i = 0; i < node->block.stmt_count; i++) {
                    print_node_tree(fd, node->block.stmts[i], tokens, ctx, i == node->block.stmt_count - 1);
                }
                ctx->depth--;
                ctx->depth--;
            } else {
                ctx->depth++;
                ensure_context_capacity(ctx);
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Statements [0/0]\n");
                ctx->depth--;
            }
            break;
        }

        case A_Defer: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Defer :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Statement:\n");
            ctx->depth++;
            print_node_tree(fd, node->defer_stmt.stmt, tokens, ctx, true);
            ctx->depth--;
            ctx->depth--;
            break;
        }

        case A_Return: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Return :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Expression:\n");
            ctx->depth++;
            print_node_tree(fd, node->return_stmt.expr, tokens, ctx, true);
            ctx->depth--;
            ctx->depth--;
            break;
        }

        case A_Continue: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Continue :: [%zu .. %zu]", node->span.start, node->span.end);

            if (node->continue_stmt.label_len > 0) {
                fprintf(fd, "\n");
                ctx->depth++;
                ensure_context_capacity(ctx);
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Label: \"%.*s\"\n", (i32)node->continue_stmt.label_len, node->continue_stmt.label_ptr);
                ctx->depth--;
            } else {
                fprintf(fd, "\n");
            }
            break;
        }

        case A_Break: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Break :: [%zu .. %zu]", node->span.start, node->span.end);

            if (node->break_stmt.label_len > 0) {
                fprintf(fd, "\n");
                ctx->depth++;
                ensure_context_capacity(ctx);
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Label: \"%.*s\"\n", (i32)node->break_stmt.label_len, node->break_stmt.label_ptr);
                ctx->depth--;
            } else {
                fprintf(fd, "\n");
            }
            break;
        }

        case A_Static: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Static :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->static_decl.name_len, node->static_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Type Span :: [%u .. %u] -> ", 
                    node->static_decl.type.start_index, node->static_decl.type.end_index);
            if (node->static_decl.type.start_index != 0) {
                print_span(fd, tokens, node->static_decl.type);
            } else {
                fprintf(fd, "<inferred>");
            }
            fprintf(fd, "\n");

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Is Const: %s\n", node->static_decl.is_const ? "true" : "false");

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Value:\n");
            ctx->depth++;
            print_node_tree(fd, node->static_decl.value, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Const: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Const :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->const_decl.name_len, node->const_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Type Span :: [%u .. %u] -> ", 
                    node->const_decl.type.start_index, node->const_decl.type.end_index);
            if (node->const_decl.type.start_index != 0) {
                print_span(fd, tokens, node->const_decl.type);
            } else {
                fprintf(fd, "<inferred>");
            }
            fprintf(fd, "\n");

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Value:\n");
            ctx->depth++;
            print_node_tree(fd, node->const_decl.value, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_VarDecl: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "VarDecl :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Name: \"%.*s\"\n", (i32)node->var_decl.name_len, node->var_decl.name_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Type Span :: [%u .. %u] -> ", 
                    node->var_decl.type.start_index, node->var_decl.type.end_index);
            print_span(fd, tokens, node->var_decl.type);
            fprintf(fd, "\n");

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Value:\n");
            ctx->depth++;
            print_node_tree(fd, node->var_decl.value, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_If: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "If :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = (node->if_stmt.else_block != nullptr);
            print_tree_indent(fd, ctx, node->if_stmt.else_block == nullptr, false);
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
                print_node_tree(fd, branch->condition, tokens, ctx, true);
                ctx->depth--;

                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Block:\n");
                ctx->depth++;
                print_node_tree(fd, branch->block, tokens, ctx, true);
                ctx->depth--;

                ctx->depth--;
            }
            ctx->depth--;

            if (node->if_stmt.else_block != nullptr) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Else Block:\n");
                ctx->depth++;
                print_node_tree(fd, node->if_stmt.else_block, tokens, ctx, true);
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Match: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Match :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->match_stmt.target, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = (node->match_stmt.default_block != nullptr);
            print_tree_indent(fd, ctx, node->match_stmt.default_block == nullptr, false);
            fprintf(fd, "Arms [%u/%u]\n", node->match_stmt.arm_count, node->match_stmt.arm_capacity);

            if (node->match_stmt.arm_count > 0) {
                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->match_stmt.arm_count; i++) {
                    AstMatchArm* arm = &node->match_stmt.arms[i];
                    bool is_last_arm = (i == node->match_stmt.arm_count - 1);
                    ctx->has_more[ctx->depth - 1] = !is_last_arm;

                    print_tree_indent(fd, ctx, is_last_arm, false);
                    fprintf(fd, "Arm %u\n", i);

                    ctx->depth++;
                    ensure_context_capacity(ctx);

                    const char* pattern_kind = arm->kind == PatternIdent ? "Ident" :
                                              (arm->kind == PatternLit ? "Literal" : "Wildcard");

                    ctx->has_more[ctx->depth - 1] = true;
                    print_tree_indent(fd, ctx, false, false);
                    fprintf(fd, "Pattern: %s :: [%zu .. %zu]", pattern_kind, arm->span.start, arm->span.end);

                    if (arm->kind == PatternIdent) {
                        fprintf(fd, " -> \"%.*s\"\n", (i32)arm->ident.len, arm->ident.ptr);
                    } else if (arm->kind == PatternLit) {
                        fprintf(fd, " -> %s: \"%.*s\"\n", TOKEN_KIND_STRINGS[arm->lit.kind], 
                                (i32)arm->lit.len, arm->lit.ptr);
                    } else {
                        fprintf(fd, "\n");
                    }

                    ctx->has_more[ctx->depth - 1] = false;
                    print_tree_indent(fd, ctx, true, false);
                    fprintf(fd, "Block:\n");
                    ctx->depth++;
                    print_node_tree(fd, arm->block, tokens, ctx, true);
                    ctx->depth--;

                    ctx->depth--;
                }
                ctx->depth--;
            }

            if (node->match_stmt.default_block != nullptr) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Default Block:\n");
                ctx->depth++;
                print_node_tree(fd, node->match_stmt.default_block, tokens, ctx, true);
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_While: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "While :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Condition:\n");
            ctx->depth++;
            print_node_tree(fd, node->while_loop.condition, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Block:\n");
            ctx->depth++;
            print_node_tree(fd, node->while_loop.block, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Loop: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Loop :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);
            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Block:\n");
            ctx->depth++;
            print_node_tree(fd, node->loop_loop.block, tokens, ctx, true);
            ctx->depth--;
            ctx->depth--;
            break;
        }

        case A_For: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "For :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Variable: \"%.*s\"\n", (i32)node->for_loop.var_len, node->for_loop.var_ptr);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Iterator:\n");
            ctx->depth++;
            print_node_tree(fd, node->for_loop.iterator, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Block:\n");
            ctx->depth++;
            print_node_tree(fd, node->for_loop.block, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Binary: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Binary :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Operator: %s\n", TOKEN_KIND_STRINGS[node->binary_op.op]);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Left:\n");
            ctx->depth++;
            print_node_tree(fd, node->binary_op.left, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Right:\n");
            ctx->depth++;
            print_node_tree(fd, node->binary_op.right, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Unary: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Unary :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Operator: %s\n", TOKEN_KIND_STRINGS[node->unary_op.op]);

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Operand:\n");
            ctx->depth++;
            print_node_tree(fd, node->unary_op.operand, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Assign: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Assign :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Operator: %s\n", TOKEN_KIND_STRINGS[node->assign.op]);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->assign.target, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Value:\n");
            ctx->depth++;
            print_node_tree(fd, node->assign.value, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_Call: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Call :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = (node->fn_call.arg_count > 0);
            print_tree_indent(fd, ctx, node->fn_call.arg_count == 0, false);
            fprintf(fd, "Function:\n");
            ctx->depth++;
            print_node_tree(fd, node->fn_call.ident, tokens, ctx, true);
            ctx->depth--;

            if (node->fn_call.arg_count > 0) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Arguments [%u/%u]\n", node->fn_call.arg_count, node->fn_call.arg_capacity);

                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->fn_call.arg_count; i++) {
                    print_node_tree(fd, node->fn_call.args[i], tokens, ctx, i == node->fn_call.arg_count - 1);
                }
                ctx->depth--;
            }

            ctx->depth--;
            break;
        }

        case A_Ident: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Ident :: [%zu .. %zu] -> \"%.*s\"\n", 
                    node->span.start, node->span.end, (i32)node->ident.len, node->ident.ptr);
            break;
        }

        case A_Literal: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Literal :: [%zu .. %zu] -> %s: \"%.*s\"\n",
                    node->span.start, node->span.end, TOKEN_KIND_STRINGS[node->literal.kind],
                    (i32)node->literal.len, node->literal.ptr);
            break;
        }

        case A_Index: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Index :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->index_access.ident, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Index:\n");
            ctx->depth++;
            print_node_tree(fd, node->index_access.index, tokens, ctx, true);
            ctx->depth--;

            ctx->depth--;
            break;
        }

        case A_MemberAccess: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "MemberAccess :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Target:\n");
            ctx->depth++;
            print_node_tree(fd, node->member_access.target, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Field: \"%.*s\"\n", (i32)node->member_access.field_len, node->member_access.field_ptr);

            ctx->depth--;
            break;
        }

        case A_Cast: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "Cast :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = true;
            print_tree_indent(fd, ctx, false, false);
            fprintf(fd, "Expression:\n");
            ctx->depth++;
            print_node_tree(fd, node->cast.expr, tokens, ctx, true);
            ctx->depth--;

            ctx->has_more[ctx->depth - 1] = false;
            print_tree_indent(fd, ctx, true, false);
            fprintf(fd, "Target Type Span :: [%u .. %u] -> ", 
                    node->cast.target_type.start_index, node->cast.target_type.end_index);
            print_span(fd, tokens, node->cast.target_type);
            fprintf(fd, "\n");

            ctx->depth--;
            break;
        }

        case A_StructInit: {
            print_tree_indent(fd, ctx, is_last, false);
            fprintf(fd, "StructInit :: [%zu .. %zu]\n", node->span.start, node->span.end);

            ctx->depth++;
            ensure_context_capacity(ctx);

            ctx->has_more[ctx->depth - 1] = (node->struct_init.field_count > 0);
            print_tree_indent(fd, ctx, node->struct_init.field_count == 0, false);
            fprintf(fd, "Type:\n");
            ctx->depth++;
            print_node_tree(fd, node->struct_init.ident, tokens, ctx, true);
            ctx->depth--;

            if (node->struct_init.field_count > 0) {
                ctx->has_more[ctx->depth - 1] = false;
                print_tree_indent(fd, ctx, true, false);
                fprintf(fd, "Field Initializers [%u/%u]\n", node->struct_init.field_count, 
                        node->struct_init.field_capacity);

                ctx->depth++;
                ensure_context_capacity(ctx);
                for (u32 i = 0; i < node->struct_init.field_count; i++) {
                    AstFieldInit* field_init = &node->struct_init.field_inits[i];
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
                    print_node_tree(fd, field_init->value, tokens, ctx, true);
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

void print_segment(FILE* fd, AstSegment segment) {
    fprintf(fd, "%.*s", (i32)segment.len, segment.ptr);
}

void print_span(FILE* fd, Tokens* tokens, AstSpan span) {
    const char* start = tokens->items[span.start_index].lexeme;
    const char* end = tokens->items[span.end_index].lexeme + tokens->items[span.end_index].length;

    const usize len = end - start;

    fprintf(fd, "%.*s", (i32) len, start);
}
