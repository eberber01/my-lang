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
    AstIfElse *if_stmt;
    AstVarDec *dec;
    AstVarAsgn *asgn;
    AstWhile *w_stmt;
    AstFor *f_stmt;
    AstExprStmt *expr_stmt;
    AstUnaryExpr *unary_expr;

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
        printlvl("\top=%s", level, bin_exp->value);

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

        printlvl("Comp Stmt(", level);
        for (size_t i = 0; i < comp_stmt->body->length; i++)
            _print_ast((AstNode *)vector_get(comp_stmt->body, i), level + 1);
        printlvl(")", level);
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
        if_stmt = (AstIfElse *)node->as;
        printlvl("If(", level);

        printlvl("\tif_expr=[", level);
        _print_ast(if_stmt->if_expr, level);
        printlvl("\t]", level);

        printlvl("\tif_body=[", level);
        _print_ast(if_stmt->if_body, level);
        printlvl("\t]", level);

        if (if_stmt->else_body != NULL)
        {
            printlvl("\telse_body=[", level);
            _print_ast(if_stmt->else_body, level);
            printlvl("\t]", level);
        }

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

        printlvl("\texpr=[", level);
        _print_ast(asgn->expr, level + 1);
        printlvl("\t]", level);

        printlvl(")", level);
        break;
    case AST_WHILE:
        w_stmt = (AstWhile *)node->as;
        printlvl("While(", level);

        printlvl("\texpr=[", level);
        _print_ast(w_stmt->expr, level + 1);
        printlvl("\t]", level);

        printlvl("\tbody=[", level);
        _print_ast(w_stmt->body, level + 1);
        printlvl("\t]", level);

        printlvl(")", level);
        break;
    case AST_FOR:
        f_stmt = (AstFor *)node->as;
        printlvl("For(", level);

        printlvl("\tinit=[", level);
        _print_ast(f_stmt->init, level + 1);
        printlvl("\t]", level);

        printlvl("\tcond=[", level);
        _print_ast(f_stmt->cond, level + 1);
        printlvl("\t]", level);

        printlvl("\tstep=[", level);
        _print_ast(f_stmt->step, level + 1);
        printlvl("\t]", level);

        printlvl("\tbody=[", level);
        _print_ast(f_stmt->body, level + 1);
        printlvl("\t]", level);
        break;
    case AST_EXPR_STMT:
        expr_stmt = node->as;
        printlvl("Expr Stmt(", level);
        _print_ast(expr_stmt->expr, level);
        printlvl(")", level);
        break;

    case AST_UNARY_EXPR:
        unary_expr = node->as;
        printlvl("Unary Expr( ", level);
        printlvl("\top=%s", level, unary_expr->value);
        printlvl("\texpr=[", level);
        _print_ast(unary_expr->postfix_expr, level + 1);
        printlvl("\t]", level);
        printlvl(")", level);
        break;
    case AST_EMPTY_EXPR:
        printlvl("Empty Expr()", level);
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

AstNode *make_ast_unary_expr(AstNode *postfix_expr, char *value, TokenType op_type)
{
    AstUnaryExpr *unary_expr = (AstUnaryExpr *)my_malloc(sizeof(AstUnaryExpr));
    unary_expr->postfix_expr = postfix_expr;
    unary_expr->value = value;
    unary_expr->op_type = op_type;
    return make_ast_node(AST_UNARY_EXPR, unary_expr);
}

AstNode *make_expr_stmt(AstNode *expr)
{
    AstExprStmt *stmt = (AstExprStmt *)my_malloc(sizeof(AstExprStmt));
    stmt->expr = expr;
    return make_ast_node(AST_EXPR_STMT, stmt);
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

AstNode *make_ast_bin_exp(char *value, TokenType op_type, AstNode *left, AstNode *right)
{
    AstBinExp *bin_exp = (AstBinExp *)my_malloc(sizeof(AstBinExp));
    bin_exp->left = left;
    bin_exp->right = right;
    bin_exp->value = value;
    bin_exp->op_type = op_type;
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

AstNode *make_ast_if_else(AstNode *if_expr, AstNode *if_body, AstNode *else_body)
{
    AstIfElse *if_stmt = (AstIfElse *)my_malloc(sizeof(AstIfElse));
    if_stmt->if_body = if_body;
    if_stmt->if_expr = if_expr;
    if_stmt->else_body = else_body;
    return make_ast_node(AST_IF, if_stmt);
}

AstNode *make_ast_while(AstNode *expr, AstNode *body)
{
    AstWhile *w_stmt = (AstWhile *)my_malloc(sizeof(AstWhile));
    w_stmt->body = body;
    w_stmt->expr = expr;
    return make_ast_node(AST_WHILE, w_stmt);
}

AstNode *make_ast_for(AstNode *init, AstNode *cond, AstNode *step, AstNode *body)
{
    AstFor *f_stmt = (AstFor *)my_malloc(sizeof(AstFor));
    f_stmt->init = init;
    f_stmt->cond = cond;
    f_stmt->step = step;
    f_stmt->body = body;
    return make_ast_node(AST_FOR, f_stmt);
}

void ast_free(AstNode *node)
{
    AstCompStmt *comp_stmt;
    AstIfElse *if_stmt;
    AstVarDef *var_def;
    AstBinExp *bin_exp;
    AstIntConst *int_const;
    AstFuncDef *func_def;
    AstIdent *ident;
    AstFuncCall *func_call;
    AstRet *ret;
    AstEnum *enm;
    AstVarDec *dec;
    AstVarAsgn *asgn;
    AstFor *f_stmt;
    AstWhile *w_stmt;
    AstExprStmt *expr_stmt;
    AstUnaryExpr *unary_expr;

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
        if_stmt = (AstIfElse *)node->as;
        ast_free(if_stmt->if_body);
        ast_free(if_stmt->if_expr);
        if (if_stmt->else_body != NULL)
            ast_free(if_stmt->else_body);
        free(if_stmt);
        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;
        ast_free(var_def->expr);
        free(var_def->value);
        free(var_def->type);
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
    case AST_WHILE:
        w_stmt = (AstWhile *)node->as;
        ast_free(w_stmt->body);
        ast_free(w_stmt->expr);
        free(w_stmt);
        break;
    case AST_FOR:
        f_stmt = (AstFor *)node->as;
        ast_free(f_stmt->init);
        ast_free(f_stmt->cond);
        ast_free(f_stmt->step);
        ast_free(f_stmt->body);
        free(f_stmt);
        break;
    case AST_EXPR_STMT:
        expr_stmt = (AstExprStmt *)node->as;
        ast_free(expr_stmt->expr);
        break;
    case AST_UNARY_EXPR:
        unary_expr = (AstUnaryExpr *)node->as;
        if (unary_expr->value != NULL)
            free(unary_expr->value);
        ast_free(unary_expr->postfix_expr);
        free(unary_expr);
        break;
    case AST_EMPTY_EXPR:
        break;
    default:
        perror("unkown ast type");
        break;
    }

    free(node);
}
