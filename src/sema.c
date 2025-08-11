#include "symtab.h"
#include "ast.h"
#include "symtab.h"
#include "util.h"
#include "sema.h"

#define INT_SIZE 4

int scope_id = 0;


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


Scope* enter_scope(Scope *parent){
    Scope *child = (Scope*)my_malloc(sizeof(Scope));
    child->table = symtab_clone(parent->table);
    child->parent = parent;
    child->id = ++scope_id;
    return child;
}

Scope* exit_scope(Scope *scope){
    Scope *tmp =scope;
    free(scope);
    return tmp->parent;
}


void scope_add(Scope *scope, SymTabEntry *entry)
{
    entry->scope_id = scope->id;
    symtab_add(scope->table, entry);
}


SymTabEntry *scope_lookup(Scope *scope, char *key){

    Scope *curr;
    SymTabEntry *entry;
    curr = scope;
    while(curr)
    {
        entry = symtab_get(curr->table, key);
        if(entry) 
            return entry;

        curr = curr->parent;
    }
    return NULL;
}

bool in_scope(Scope *scope, char *key)
{
    SymTabEntry *entry = scope_lookup(scope, key);
    return entry && entry->scope_id == scope->id;
}

void sym_check(AstNode* node, StackFrame *frame, Scope *scope){

    AstCompStmt *comp_stmt;
    AstIf *if_stmt;
    AstVarDef *var_def;
    AstBinExp *bin_exp;
    AstFuncDef *func_def;
    AstBoolExpr *bool_expr;
    AstIdent *ident;
    AstFuncCall *func_call;
    AstRet *ret;
    SymTabEntry *entry;
    SymTabEntry *entry_type;

    switch (node->type)
    {
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;
        Scope *child = enter_scope(scope); 
        for(size_t i=0; i< comp_stmt->body->length; i++)
            sym_check((AstNode*)vector_get(comp_stmt->body, i), frame, child);
        exit_scope(child);
        break;
    case AST_IF:
        if_stmt = (AstIf *)node->as;
        sym_check(if_stmt->expr, frame, scope);
        sym_check(if_stmt->body, frame, scope);
        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;

        // Check if symbol exists in scope
        if (in_scope(scope, var_def->value))
        {
            perror("Redefinition of variable.");
            exit(1);
        }

        sym_check(var_def->expr, frame, scope);

        entry_type = scope_lookup(scope, var_def->type);
        entry = make_symtab_entry(str_clone(var_def->value), entry_type->type, SYM_VARIABLE);
        entry->offset = stackframe_add(frame);
        var_def->symbol = entry;

        scope_add(scope, entry);
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        sym_check(bin_exp->left, frame, scope);
        sym_check(bin_exp->right, frame, scope);
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
        for(size_t i = 0; i < func_def->params->length; i++){
            Param * param = (Param*)vector_get(func_def->params, i);
            entry_type = scope_lookup(scope, param->type);
            entry = make_symtab_entry(str_clone(param->value), entry_type->type, SYM_VARIABLE);
            param->symbol = entry;
            scope_add(scope, entry);
        }

        // Insert params into symbol table
        TypeSpecifier ret_type = scope_lookup(scope, func_def->type)->type;
        SymTabEntry *entry = make_symtab_entry(str_clone(func_def->value), ret_type, SYM_FUNCTION);

        sym_check(func_def->body, frame, scope);

        entry->params = func_def->params;
        entry->frame = frame;
        func_def->symbol = entry;

        scope_add(scope, entry);

        break;
    case AST_BOOL_EXPR:
        bool_expr = (AstBoolExpr *)node->as;
        sym_check(bool_expr->left, frame, scope);
        sym_check(bool_expr->right, frame, scope);
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
        func_call = (AstFuncCall*)node->as;
        entry = scope_lookup(scope, func_call->value);

        Vector* args = func_call->args;
        Vector *params = entry->params;
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

        for(size_t i=0; i < func_call->args->length; i++)
            sym_check((AstNode*)vector_get(func_call->args, i), frame, scope);


        break;
    case AST_RET:
        ret = (AstRet*)node->as;
        ret->func = frame->func;
        sym_check(ret->expr, frame, scope);
        break;
    default:
        perror("unkown ast type");
        break;
    }
}

void sema_check(Vector *prog, SymTab *table){
    AstNode* node;
    Scope* global = (Scope*)my_malloc(sizeof(Scope));
    global->table =table;
    global->id = 0;
    for(size_t i=0; i < prog->length; i++)
    {
        node = (AstNode*)vector_get(prog, i);
        sym_check(node, NULL, global);
    }

}
