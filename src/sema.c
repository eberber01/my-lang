#include "sema.h"
#include "ast.h"
#include "hashmap.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

#define INT_SIZE 4

StackFrame *make_stack_frame(char *func)
{
    StackFrame *frame = my_malloc(sizeof(StackFrame));

    frame->size = 0;
    frame->func = func;
    return frame;
}

// Returns variable offset from current stack pointer
int stackframe_add(StackFrame *frame)
{
    // Change for different Types
    int tmp = frame->size;
    frame->size += INT_SIZE;
    return tmp;
}

Scope *enter_scope(Scope *parent)
{
    Scope *child = (Scope *)my_malloc(sizeof(Scope));
    child->symtab = symtab_clone(parent->symtab);
    child->parent = parent;
    return child;
}

Scope *exit_scope(Scope *scope)
{
    Scope *parent = scope->parent;
    hashmap_free(scope->symtab);
    free(scope);
    return parent;
}

void scope_add(Scope *scope, SymTabEntry *entry)
{
    entry->scope_id = scope->id;
    hashmap_add(scope->symtab, entry, entry->key);
}

SymTabEntry *scope_lookup(Scope *scope, char *key)
{

    Scope *curr;
    SymTabEntry *entry;
    curr = scope;
    while (curr != NULL)
    {
        entry = hashmap_get(curr->symtab, key);
        if (entry)
            return entry;

        curr = curr->parent;
    }
    return NULL;
}

bool in_scope(Scope *scope, char *key)
{
    SymTabEntry *entry = scope_lookup(scope, key);
    return entry;
}

void sym_check(AstNode *node, StackFrame *frame, Scope *scope, HashMap *type_env, Vector *symbols)
{

    AstCompStmt *comp_stmt;
    AstIfElse *if_stmt;
    AstVarDef *var_def;
    AstBinExp *bin_exp;
    AstFuncDef *func_def;
    AstIdent *ident;
    AstFuncCall *func_call;
    AstRet *ret;
    AstEnum *enm;
    AstVarDec *dec;
    AstVarAsgn *asgn;
    AstWhile *w_stmt;
    AstFor *f_stmt;
    SymTabEntry *entry;
    TypeEnvEntry *entry_type;
    Scope* child;

    switch (node->type)
    {
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;
        child = enter_scope(scope);
        for (size_t i = 0; i < comp_stmt->body->length; i++)
            sym_check((AstNode *)vector_get(comp_stmt->body, i), frame, child, type_env, symbols);
        exit_scope(child);
        break;
    case AST_IF:
        if_stmt = (AstIfElse *)node->as;
        sym_check(if_stmt->if_body, frame, scope, type_env, symbols);
        sym_check(if_stmt->if_expr, frame, scope, type_env, symbols);
        if (if_stmt->else_body != NULL)
            sym_check(if_stmt->else_body, frame, scope, type_env, symbols);

        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;
        // Check if symbol exists in scope
        if (in_scope(scope, var_def->value))
        {
            perror("Redefinition of variable.");
            exit(1);
        }

        sym_check(var_def->expr, frame, scope, type_env, symbols);

        entry_type = hashmap_get(type_env, var_def->type);
        entry = make_symtab_entry(str_clone(var_def->value), entry_type->ts, SYM_VARIABLE);
        vector_push(symbols, entry);
        entry->offset = stackframe_add(frame);
        var_def->symbol = entry;

        scope_add(scope, entry);
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        sym_check(bin_exp->left, frame, scope, type_env, symbols);
        sym_check(bin_exp->right, frame, scope, type_env, symbols);
        break;
    case AST_INT_CONST:
        break;
    case AST_FUNC_DEF:
        func_def = (AstFuncDef *)node->as;
        // Check if symbol exists in table
        if (in_scope(scope, func_def->value))
        {
            fprintf(stderr, "Redefinition of function '%s'\n", func_def->value);
            exit(1);
        }

        frame = make_stack_frame(func_def->value);

        // Check params
        for (size_t i = 0; i < func_def->params->length; i++)
        {
            Param *param = (Param *)vector_get(func_def->params, i);
            entry_type = hashmap_get(type_env, param->type);
            entry = make_symtab_entry(str_clone(param->value), entry_type->ts, SYM_VARIABLE);
            param->symbol = entry;
            scope_add(scope, entry);
        }

        // Insert params into symbol table
        TypeEnvEntry *ret_type = hashmap_get(type_env, func_def->type);
        SymTabEntry *entry = make_symtab_entry(str_clone(func_def->value), ret_type->ts, SYM_FUNCTION);
        vector_push(symbols, entry);

        entry->params = func_def->params;
        entry->frame = frame;
        func_def->symbol = entry;
        scope_add(scope, entry);

        sym_check(func_def->body, frame, scope, type_env, symbols);

        break;
    case AST_IDENT:
        ident = (AstIdent *)node->as;
        entry = scope_lookup(scope, ident->value);
        if (entry == NULL)
        {
            perror("Cannot used undefined variable");
            exit(1);
        }

        ident->symbol = entry;
        break;
    case AST_FUNC_CALL:
        func_call = (AstFuncCall *)node->as;
        entry = scope_lookup(scope, func_call->value);

        Vector *args = func_call->args;
        Vector *params = entry->params;
        func_call->symbol = entry;
        if (entry == NULL)
        {
            fprintf(stderr, "Undefined function '%s'\n", func_call->value);
            exit(1);
        }

        if (params->length != args->length)
        {
            fprintf(stderr, "Mismatching args for function '%s'\n", func_call->value);
            exit(1);
        }

        for (size_t i = 0; i < func_call->args->length; i++)
            sym_check((AstNode *)vector_get(func_call->args, i), frame, scope, type_env, symbols);

        break;
    case AST_RET:
        ret = (AstRet *)node->as;
        ret->func = frame->func;
        sym_check(ret->expr, frame, scope, type_env, symbols);
        break;
    case AST_ENUM:
        enm = (AstEnum *)node->as;
        for (size_t i = 0; i < enm->enums->length; i++)
        {
            entry = make_symtab_entry(vector_get(enm->enums, i), TS_INT, SYM_CONST);
            entry->const_value = i;
            scope_add(scope, entry);
        }
        break;
    case AST_VAR_DEC:
        dec = (AstVarDec *)node->as;
        // Check if symbol exists in scope
        if (in_scope(scope, dec->value))
        {
            perror("Redeclaration of variable.");
            exit(1);
        }

        entry_type = hashmap_get(type_env, dec->type);
        entry = make_symtab_entry(str_clone(dec->value), entry_type->ts, SYM_VARIABLE);
        entry->offset = stackframe_add(frame);
        dec->symbol = entry;
        scope_add(scope, entry);
        break;
    case AST_VAR_ASGN:
        asgn = (AstVarAsgn *)node->as;

        // Check if symbol exists in scope
        if (!in_scope(scope, asgn->value))
        {
            perror("Undefined variable.");
            exit(1);
        }

        sym_check(asgn->expr, frame, scope, type_env, symbols);
        asgn->symbol = scope_lookup(scope, asgn->value);
        break;
    case AST_WHILE:
        w_stmt = (AstWhile *)node->as;

        sym_check(w_stmt->expr, frame, scope, type_env, symbols);
        sym_check(w_stmt->body, frame, scope, type_env, symbols);

        break;

    case AST_FOR:
        f_stmt = (AstFor *)node->as;
        child = enter_scope(scope);

        sym_check(f_stmt->init, frame, child, type_env, symbols);
        sym_check(f_stmt->cond, frame, child, type_env, symbols);
        sym_check(f_stmt->step, frame, child, type_env, symbols);
        sym_check(f_stmt->body, frame, child, type_env, symbols);

        exit_scope(child);
        break;
    case AST_EMPTY_EXPR:
        break;
    default:
        printf("type%d", node->type);
        perror("unkown ast type");
        exit(1);
    }
}

Vector *sema_check(Vector *prog, HashMap *type_env)
{
    AstNode *node;
    HashMap *symtab = hashmap_new();
    Vector *symbols = vector_new();

    Scope *global = (Scope *)my_malloc(sizeof(Scope));
    global->symtab = symtab;
    global->parent = NULL;
    global->id = 0;
    for (size_t i = 0; i < prog->length; i++)
    {
        node = (AstNode *)vector_get(prog, i);
        sym_check(node, NULL, global, type_env, symbols);
    }
    exit_scope(global);
    return symbols;
}
