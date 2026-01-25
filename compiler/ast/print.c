#include "ast.h"

#include "types.h"

#include <stdio.h>

/* ============================================================
   Helpers
   ============================================================ */

static void indent(int d) {
    for (int i = 0; i < d; i++) fputs("  ", stdout);
}

static void print_bytespan(ByteSpan s) {
    printf("[bytes %zu..%zu]", s.start, s.end);
}

static void print_typespan(AstSpan s) {
    if (s.start_index == 0 && s.end_index == 0)
        printf("<none>");
    else
        printf("[tokens %u..%u]", s.start_index, s.end_index);
}

static void print_ident(const char* p, usize l) {
    if (!p || l == 0) printf("<anonymous>");
    else printf("'%.*s'", (int)l, p);
}

/* ============================================================
   Forward declarations
   ============================================================ */

static void print_node(AstNode* n, int d);
static void print_expr(AstNode* n, int d);

/* ============================================================
   Expression printer
   ============================================================ */

static void print_expr(AstNode* n, int d) {
    if (!n) {
        indent(d); puts("<null expr>");
        return;
    }

    indent(d);
    printf("Expr %s ", AST_KIND_STRINGS[n->kind]);
    print_bytespan(n->span);
    putchar('\n');

    switch (n->kind) {

    case A_Ident:
        indent(d + 1);
        printf("identifier: ");
        print_ident(n->ident.ptr, n->ident.len);
        putchar('\n');
        break;

    case A_Literal:
        indent(d + 1);
        printf("literal token: %s\n",
               TOKEN_KIND_STRINGS[n->literal.kind]);
        indent(d + 1);
        printf("value: '%.*s'\n",
               (int)n->literal.len,
               n->literal.ptr);
        break;

    case A_Binary:
        indent(d + 1);
        printf("operator: %s\n",
               TOKEN_KIND_STRINGS[n->binary_op.op]);
        indent(d + 1); puts("left:");
        print_expr(n->binary_op.left, d + 2);
        indent(d + 1); puts("right:");
        print_expr(n->binary_op.right, d + 2);
        break;

    case A_Unary:
        indent(d + 1);
        printf("operator: %s\n",
               TOKEN_KIND_STRINGS[n->unary_op.op]);
        print_expr(n->unary_op.operand, d + 1);
        break;

    case A_Assign:
        indent(d + 1);
        printf("operator: %s\n",
               TOKEN_KIND_STRINGS[n->assign.op]);
        indent(d + 1); puts("target:");
        print_expr(n->assign.target, d + 2);
        indent(d + 1); puts("value:");
        print_expr(n->assign.value, d + 2);
        break;

    case A_Call:
        indent(d + 1); puts("callee:");
        print_expr(n->fn_call.ident, d + 2);
        indent(d + 1);
        printf("arguments (%u):\n", n->fn_call.arg_count);
        for (u32 i = 0; i < n->fn_call.arg_count; i++)
            print_expr(n->fn_call.args[i], d + 2);
        break;

    case A_Index:
        indent(d + 1); puts("target:");
        print_expr(n->index_access.ident, d + 2);
        indent(d + 1); puts("index:");
        print_expr(n->index_access.index, d + 2);
        break;

    case A_Cast:
        indent(d + 1);
        printf("target type: ");
        print_typespan(n->cast.target_type);
        putchar('\n');
        print_expr(n->cast.expr, d + 1);
        break;

    case A_StructInit:
        indent(d + 1); puts("struct:");
        print_expr(n->struct_init.ident, d + 2);
        indent(d + 1);
        printf("fields (%u):\n", n->struct_init.field_count);
        for (u32 i = 0; i < n->struct_init.field_count; i++) {
            AstFieldInit* f = &n->struct_init.field_inits[i];
            indent(d + 2);
            printf("field ");
            print_ident(f->ptr, f->len);
            putchar('\n');
            print_expr(f->value, d + 3);
        }
        break;

    default:
        indent(d + 1);
        puts("<expr details not implemented>");
        break;
    }
}

/* ============================================================
   Node printer (ALL NODES)
   ============================================================ */

static void print_node(AstNode* n, int d) {
    if (!n) {
        indent(d); puts("<null node>");
        return;
    }

    indent(d);
    printf("%s ", AST_KIND_STRINGS[n->kind]);
    print_bytespan(n->span);
    putchar('\n');

    switch (n->kind) {

    /* ---------- Decls ---------- */

    case A_VarDecl: {
        AstVarDecl* v = &n->var_decl;
        indent(d + 1); printf("identifier: ");
        print_ident(v->name_ptr, v->name_len); putchar('\n');
        indent(d + 1); printf("type: ");
        print_typespan(v->type); putchar('\n');
        indent(d + 1);
        if (v->value) {
            puts("initialised value:");
            print_expr(v->value, d + 2);
        } else {
            puts("initialised value: <none>");
        }
        break;
    }

    case A_Const:
        indent(d + 1); printf("identifier: ");
        print_ident(n->const_decl.name_ptr,
                    n->const_decl.name_len); putchar('\n');
        indent(d + 1); printf("type: ");
        print_typespan(n->const_decl.type); putchar('\n');
        indent(d + 1); puts("value:");
        print_expr(n->const_decl.value, d + 2);
        break;

    case A_Static:
        indent(d + 1); printf("identifier: ");
        print_ident(n->static_decl.name_ptr,
                    n->static_decl.name_len); putchar('\n');
        indent(d + 1); printf("type: ");
        print_typespan(n->static_decl.type); putchar('\n');
        indent(d + 1); puts("value:");
        print_expr(n->static_decl.value, d + 2);
        break;

    case A_Function: {
        AstFunction* f = &n->function_decl;
        indent(d + 1); printf("name: ");
        print_ident(f->name_ptr, f->name_len); putchar('\n');
        indent(d + 1);
        printf("params (%u)\n", f->param_count);
        indent(d + 1); printf("return type: ");
        print_typespan(f->return_type); putchar('\n');
        indent(d + 1); puts("body:");
        print_node(f->body, d + 2);
        break;
    }

    /* ---------- Control ---------- */

    case A_Block:
        indent(d + 1);
        printf("statements (%zu):\n", n->block.stmt_count);
        for (usize i = 0; i < n->block.stmt_count; i++)
            print_node(n->block.stmts[i], d + 2);
        break;

    case A_If:
        for (u32 i = 0; i < n->if_stmt.branch_count; i++) {
            indent(d + 1);
            printf("if-branch %u:\n", i);
            print_expr(n->if_stmt.branches[i].condition, d + 2);
            print_node(n->if_stmt.branches[i].block, d + 2);
        }
        if (n->if_stmt.else_block) {
            indent(d + 1); puts("else:");
            print_node(n->if_stmt.else_block, d + 2);
        }
        break;

    case A_While:
        indent(d + 1); puts("condition:");
        print_expr(n->while_loop.condition, d + 2);
        indent(d + 1); puts("block:");
        print_node(n->while_loop.block, d + 2);
        break;

    case A_For:
        indent(d + 1); printf("var: ");
        print_ident(n->for_loop.var_ptr,
                    n->for_loop.var_len); putchar('\n');
        indent(d + 1); puts("iterator:");
        print_expr(n->for_loop.iterator, d + 2);
        indent(d + 1); puts("block:");
        print_node(n->for_loop.block, d + 2);
        break;

    case A_Loop:
        print_node(n->loop_loop.block, d + 1);
        break;

    case A_Return:
        if (n->return_stmt.expr)
            print_expr(n->return_stmt.expr, d + 1);
        break;

    case A_Defer:
        print_node(n->defer_stmt.stmt, d + 1);
        break;

    case A_Break:
    case A_Continue:
        indent(d + 1); printf("label: ");
        print_ident(n->break_stmt.label_ptr,
                    n->break_stmt.label_len);
        putchar('\n');
        break;

    default:
        indent(d + 1);
        puts("<node details omitted>");
        break;
    }
}

/* ============================================================
   Entry point
   ============================================================ */

void print_ast(ParseTree* tree) {
    if (!tree) {
        puts("<null ParseTree>");
        return;
    }

    printf("ParseTree (%zu nodes)\n", tree->count);
    puts("================================");

    for (usize i = 0; i < tree->count; i++)
        print_node(tree->nodes[i], 0);
}
