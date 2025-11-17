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
    AstEnum *enm;
    AstIf *if_stmt;
    AstBoolExpr *bool_expr;
    AstVarDec *dec;
    AstVarAsgn *asgn;

    switch (node->type)
    {
    case AST_FUNC_DEF:
        func_def = (AstFuncDef *)node->as;
        printlvl("FuncDef(", level);

        printlvl("\tname='%s'", level, func_def->value);
        printlvl("\tscope_id='%d'", level, func_def->symbol->scope_id);
        printlvl("\tframe_size='%d'", level, func_def->symbol->frame->size);
        printlvl("\tparams=[", level, func_def->params);
        for (size_t i = 0; i < func_def->params->length; i++)
        {
            printlvl("\t\t'%s'", level, ((Param *)vector_get(func_def->params, i))->value);
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
        printlvl("\tscope_id='%d'", level, var_def->symbol->scope_id);
        printlvl("\tstack_offset='%d'", level, var_def->symbol->offset);
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
        printlvl("Ident(", level);
        printlvl("\tname='%s'", level, ident->value);
        printlvl("\tscope_id='%d'", level, ident->symbol->scope_id);
        printlvl(")", level);
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
        printlvl("\tscope_id='%d'", level, func_call->symbol->scope_id);
        printlvl("\targs=(", level);
        for (size_t i = 0; i < func_call->args->length; i++)
            _print_ast(vector_get(func_call->args, i), level + 1);
        printlvl("\t)", level);
        break;
    case AST_ENUM:
        enm = (AstEnum *)node->as;
        printlvl("Enum(", level);
        printlvl("\tname=%s", level, enm->value);
        printlvl("\tenums=[", level);
        for (size_t i = 0; i < enm->enums->length; i++)
            printlvl("\t\t%s(%d),", level, ((char *)vector_get(enm->enums, i)), i);
        printlvl("\t]", level);
        printlvl(")", level);
        break;
    case AST_IF:
        if_stmt = (AstIf *)node->as;
        printlvl("If(", level);
        printlvl("\texpr=[", level);
        _print_ast(if_stmt->expr, level);
        printlvl("\t]", level);
        printlvl("\tbody=[", level);
        _print_ast(if_stmt->body, level);
        printlvl("\t]", level);
        break;
    case AST_BOOL_EXPR:
        bool_expr = (AstBoolExpr *)node->as;
        printlvl("BoolExpr(", level);
        printlvl("\toperator='%s'", level, bool_expr->value);

        printlvl("\tleft=(", level);
        _print_ast(bool_expr->left, level + 1);
        printlvl("\t)", level);

        printlvl("\tright=(", level);
        _print_ast(bool_expr->right, level + 1);
        printlvl("\t)", level);

        break;
    case AST_VAR_DEC:
        dec = (AstVarDec *)node->as;

        printlvl("VarDec(", level);
        printlvl("\tvalue='%s'", level, dec->value);
        printlvl("\tstack_offset='%d'", level, dec->symbol->offset);
        printlvl("\ttype='%s'", level, dec->type);
        printlvl(")", level);
        break;

    case AST_VAR_ASGN:
        asgn = (AstVarAsgn *)node->as;

        printlvl("VarAsgn(", level);
        printlvl("\tvalue='%s'", level, asgn->value);
        printlvl("\tstack_offset='%d'", level, asgn->symbol->offset);

        printlvl("\texpr=[", level, asgn->value);
        _print_ast(asgn->expr, level + 1);
        printlvl("\t]", level, asgn->value);

        printlvl(")", level);
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

AstNode *make_ast_enum(char *value, Vector *enums)
{
    AstEnum *enm = (AstEnum *)my_malloc(sizeof(AstEnum));
    enm->enums = enums;
    enm->value = value;
    return make_ast_node(AST_ENUM, enm);
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

AstNode *make_ast_var_dec(char *value, char *type)
{
    AstVarDec *dec = (AstVarDec *)my_malloc(sizeof(AstVarDec));
    dec->value = value;
    dec->type = type;
    return make_ast_node(AST_VAR_DEC, dec);
}

AstNode *make_ast_var_asgn(char *value, AstNode *expr)
{
    AstVarAsgn *asgn = (AstVarAsgn *)my_malloc(sizeof(AstVarAsgn));
    asgn->value = value;
    asgn->expr = expr;
    return make_ast_node(AST_VAR_ASGN, asgn);
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
    AstEnum *enm;
    AstVarDec *dec;
    AstVarAsgn *asgn;

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
    case AST_ENUM:
        enm = (AstEnum *)node->as;
        for (size_t i = 0; i < enm->enums->length; i++)
        {
            free(vector_get(enm->enums, i));
        }
        vector_free(enm->enums);
        free(enm);
        break;
    case AST_VAR_DEC:
        dec = (AstVarDec *)node->as;
        free(dec->value);
        free(dec->type);
        free(dec);
        break;
    case AST_VAR_ASGN:
        asgn = (AstVarAsgn *)node->as;
        ast_free(asgn->expr);
        free(asgn->value);
        free(asgn);
        break;
    default:
        perror("unkown ast type");
        break;
    }

    free(node);
}
