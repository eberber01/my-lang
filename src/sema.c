#include "symtab.h"
#include "ast.h"
#include "symtab.h"
#include "util.h"


void sym_check(AstNode* node, SymTab *table){

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
        for(size_t i=0; i< comp_stmt->body->length; i++)
            sym_check((AstNode*)vector_get(comp_stmt->body, i), table);
        break;
    case AST_IF:
        if_stmt = (AstIf *)node->as;
        sym_check(if_stmt->expr, table);
        sym_check(if_stmt->body, table);
        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;
        // Check if symbol exists in table
        if (symtab_get(table, var_def->value))
        {
            perror("Redefinition of variable.");
            exit(1);
        }

        sym_check(var_def->expr, table);
        entry_type = symtab_get(table, var_def->type);
        entry = make_symtab_entry(str_clone(var_def->value), entry_type->type, SYM_VARIABLE);
        symtab_add(table, entry);
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        sym_check(bin_exp->left, table);
        sym_check(bin_exp->right, table);
        break;
    case AST_INT_CONST:
        break;
    case AST_FUNC_DEF:
        func_def = (AstFuncDef *)node->as;

        // Check if symbol exists in table
        if (symtab_get(table, func_def->value))
        {
            fprintf(stderr, "Redefinition of function '%s'\n", func_def->value);
            exit(1);
        }

        // Check params
        for(size_t i = 0; i < func_def->params->length; i++){
            Param * param = (Param*)vector_get(func_def->params, i);
            entry_type = symtab_get(table, param->type);
            entry = make_symtab_entry(str_clone(param->value), entry_type->type, SYM_VARIABLE);
            symtab_add(table, entry);
        }

        // Insert params into symbol table
        TypeSpecifier ret_type = symtab_get(table, func_def->type)->type;
        SymTabEntry *entry = make_symtab_entry(str_clone(func_def->value), ret_type, SYM_FUNCTION);
        entry->params = func_def->params;
        symtab_add(table, entry);

        sym_check(func_def->body, table);
        break;
    case AST_BOOL_EXPR:
        bool_expr = (AstBoolExpr *)node->as;
        sym_check(bool_expr->left, table);
        sym_check(bool_expr->right, table);
        break;
    case AST_IDENT:
        ident = (AstIdent *)node->as;

        if (symtab_get(table, ident->value) == NULL)
        {
            perror("Cannot used undefined variable");
            exit(1);
        }

        break;
    case AST_FUNC_CALL:
        func_call = (AstFuncCall*)node->as;
        entry = symtab_get(table, func_call->value);

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

        break;
    case AST_RET:
        ret = (AstRet*)node->as;
        sym_check(ret->expr, table);
        break;
    default:
        perror("unkown ast type");
        break;
    }
}

void sema_check(Vector *prog, SymTab *table){
    AstNode* node;
    for(size_t i=0; i < prog->length; i++)
    {
        node = (AstNode*)vector_get(prog, i);
        sym_check(node, table);
    }

}
