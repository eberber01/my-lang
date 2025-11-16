#include "ast.h"

#include "util.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void printlvl(char *str, int level, ...)
{
    va_list args;
    va_start(args, level);
    for (int i = 0; i < level; i++)
        printf("\t");
    vprintf(str, args);
    printf("\n");
}

void _print_ast(AstNode *node, int level)
{

    AstCompStmt *comp_stmt;
    AstVarDef *var_def;
    AstBinExp *bin_exp;
    AstFuncDef *func_def;
    AstIdent *ident;
    AstFuncCall *func_call;
    AstRet *ret;
    AstIntConst *cons;

    switch (node->type)
    {
    case AST_FUNC_DEF:
        func_def = (AstFuncDef *)node->as;
        printlvl("FuncDef(", level);

        printlvl("\tname='%s'", level, func_def->value);
        printlvl("\tparams=[", level, func_def->params);
        for (size_t i = 0; i < func_def->params->length; i++)
        {
            printlvl("\t\t'%s'", level, (char *)vector_get(func_def->params, i));
        }
        printlvl("\t\t]", level);

        printlvl("\tbody=[", level);
        _print_ast(func_def->body, level + 1);

        printlvl("\t\t]", level);
        printlvl(")", level);
        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;
        printlvl("VarDef(", level);

        printlvl("\tname='%s'", level, var_def->value);

        printlvl("\tvalue=(", level);
        _print_ast(var_def->expr, level + 1);
        printlvl(")", level);
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        printlvl("BinExp(", level);
        printlvl("\top=%c", level, *(bin_exp->value));

        printlvl("\tleft=(", level);
        _print_ast(bin_exp->left, level + 1);
        printlvl("\t)", level);

        printlvl("\tright=(", level);
        _print_ast(bin_exp->right, level + 1);
        printlvl("\t)", level);

        printlvl(")", level);
        break;

    case AST_INT_CONST:
        cons = (AstIntConst *)node->as;
        printlvl("IntConst(%d)", level, cons->value);
        break;
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;
        for (size_t i = 0; i < comp_stmt->body->length; i++)
            _print_ast((AstNode *)vector_get(comp_stmt->body, i), level);
        break;
    case AST_IDENT:
        ident = (AstIdent *)node->as;
        printlvl("Ident(%s)", level, ident->value);
        break;

    case AST_RET:
        ret = (AstRet *)node->as;
        printlvl("Return(", level);
        _print_ast(ret->expr, level + 1);
        printlvl(")", level);
        break;
    case AST_FUNC_CALL:
        func_call = (AstFuncCall *)node->as;
        printlvl("Func Call(", level);
        printlvl("\tname=%s", level, func_call->value);
        printlvl("\targs=(", level);
        for (size_t i = 0; i < func_call->args->length; i++)
            _print_ast(vector_get(func_call->args, i), level + 1);
        printlvl("\t)", level);
        break;
    default:
        printf("type%d", node->type);
        perror("not impl");
    }
}

void print_ast(Vector *prog)
{
    for (size_t i = 0; i < prog->length; i++)
    {

        _print_ast((AstNode *)vector_get(prog, i), 0);
    }
}

AstNode *make_ast_node(AstNodeType type, void *inner)
{
    AstNode *node = (AstNode *)my_malloc(sizeof(AstNode));
    node->type = type;
    node->as = inner;
    return node;
}

AstNode *make_ast_comp_stmt(Vector *body)
{
    AstCompStmt *stmt = (AstCompStmt *)my_malloc(sizeof(AstCompStmt));
    stmt->body = body;
    return make_ast_node(AST_COMP_STMT, stmt);
}

AstNode *make_int_const(int value)
{
    AstIntConst *int_const = (AstIntConst *)my_malloc(sizeof(AstIntConst));
    int_const->value = value;
    return make_ast_node(AST_INT_CONST, int_const);
}

AstNode *make_ast_bin_exp(char *value, AstNode *left, AstNode *right)
{
    AstBinExp *bin_exp = (AstBinExp *)my_malloc(sizeof(AstBinExp));
    bin_exp->left = left;
    bin_exp->right = right;
    bin_exp->value = value;
    return make_ast_node(AST_BIN_EXP, bin_exp);
}

AstNode *make_ast_func_def(char *value, char *type, AstNode *body, Vector *params)
{
    AstFuncDef *func_def = (AstFuncDef *)my_malloc(sizeof(AstFuncDef));
    func_def->body = body;
    func_def->value = value;
    func_def->type = type;
    func_def->params = params;
    return make_ast_node(AST_FUNC_DEF, func_def);
}

AstNode *make_ast_ident(char *value)
{
    AstIdent *ident = (AstIdent *)my_malloc(sizeof(AstIdent));
    ident->value = value;
    return make_ast_node(AST_IDENT, ident);
}

AstNode *make_ast_ret(AstNode *expr)
{
    AstRet *ret = (AstRet *)my_malloc(sizeof(AstRet));
    ret->expr = expr;
    return make_ast_node(AST_RET, ret);
}

AstNode *make_ast_func_call(char *value, Vector *args)
{
    AstFuncCall *func_call = (AstFuncCall *)my_malloc(sizeof(AstFuncCall));
    func_call->value = value;
    func_call->args = args;
    return make_ast_node(AST_FUNC_CALL, func_call);
}

AstNode *make_ast_var_def(char *value, char *type, AstNode *expr)
{
    AstVarDef *var_def = (AstVarDef *)my_malloc(sizeof(AstVarDef));
    var_def->value = value;
    var_def->type = type;
    var_def->expr = expr;
    return make_ast_node(AST_VAR_DEF, var_def);
}

AstNode *make_ast_bool_expr(char *value, AstNode *left, AstNode *right)
{
    AstBoolExpr *bool_expr = (AstBoolExpr *)my_malloc(sizeof(AstBoolExpr));
    bool_expr->left = left;
    bool_expr->right = right;
    bool_expr->value = value;
    return make_ast_node(AST_BOOL_EXPR, bool_expr);
}

AstNode *make_ast_if(AstNode *expr, AstNode *body)
{
    AstIf *if_stmt = (AstIf *)my_malloc(sizeof(AstIf));
    if_stmt->body = body;
    if_stmt->expr = expr;
    return make_ast_node(AST_IF, if_stmt);
}

void ast_free(AstNode *node)
{
    AstCompStmt *comp_stmt;
    AstIf *if_stmt;
    AstVarDef *var_def;
    AstBinExp *bin_exp;
    AstIntConst *int_const;
    AstFuncDef *func_def;
    AstBoolExpr *bool_expr;
    AstIdent *ident;
    AstFuncCall *func_call;
    AstRet *ret;

    if (node == NULL)
        return;
    switch (node->type)
    {
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;
        for (size_t i = 0; i < comp_stmt->body->length; i++)
            ast_free(vector_get(comp_stmt->body, i));

        vector_free(comp_stmt->body);
        free(comp_stmt);
        break;
    case AST_IF:
        if_stmt = (AstIf *)node->as;
        ast_free(if_stmt->body);
        ast_free(if_stmt->expr);
        free(if_stmt);
        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;
        ast_free(var_def->expr);
        free(var_def->value);
        free(var_def);
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        ast_free(bin_exp->left);
        ast_free(bin_exp->right);
        free(bin_exp->value);
        free(bin_exp);
        break;
    case AST_INT_CONST:
        int_const = (AstIntConst *)node->as;
        free(int_const);
        break;
    case AST_FUNC_DEF:
        func_def = (AstFuncDef *)node->as;
        for (size_t i = 0; i < func_def->params->length; i++)
        {
            Param *param = vector_get(func_def->params, i);
            free(param->type);
            free(param->value);
            free(param);
        }
        vector_free(func_def->params);
        ast_free(func_def->body);
        free(func_def->value);
        free(func_def->type);
        free(func_def);
        break;
    case AST_BOOL_EXPR:
        bool_expr = (AstBoolExpr *)node->as;
        ast_free(bool_expr->left);
        ast_free(bool_expr->right);
        free(bool_expr->value);
        free(bool_expr);
        break;
    case AST_IDENT:
        ident = (AstIdent *)node->as;
        free(ident->value);
        free(ident);
        break;
    case AST_FUNC_CALL:
        func_call = (AstFuncCall *)node->as;
        free(func_call->value);
        for (size_t i = 0; i < func_call->args->length; i++)
            ast_free((AstNode *)vector_get(func_call->args, i));
        vector_free(func_call->args);
        free(func_call);
        break;
    case AST_RET:
        ret = (AstRet *)node->as;
        ast_free(ret->expr);
        free(ret);
        break;
    default:
        perror("unkown ast type");
        break;
    }

    free(node);
}
