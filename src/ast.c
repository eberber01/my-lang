#include <mylang/ast.h>
#include <mylang/util.h>
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
    AstLValue *lval;

    switch (node->type)
    {
    case AST_FUNC_DEF:
        func_def = (AstFuncDef *)node->as;
        printlvl("FuncDef(", level);

        printlvl("\tname='%s'", level, func_def->value);
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

        printlvl("\tlval=[", level);
        _print_ast(asgn->lval, level + 1);
        printlvl("\t]", level);

        printlvl("\trval=[", level);
        _print_ast(asgn->rval, level + 1);
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

    case AST_LVAL:
        lval = (AstLValue *)node->as;
        switch (lval->kind)
        {
        case AST_LVAL_IDENT:
            _print_ast(lval->u.ident, level);
            break;
        default:
            fprintf(stderr, "Print Ast: Unknown AST LValue Type (%d)\n", lval->kind);
            break;
        }
        break;
    case AST_EMPTY_EXPR:
        printlvl("Empty Expr()", level);
        break;
    default:
        fprintf(stderr, "Print Ast: Unknown AST Type (%d)\n", node->type);
        break;
    }
}

void dump_ast(Vector *prog)
{
    for (size_t i = 0; i < prog->length; i++)
    {

        _print_ast((AstNode *)vector_get(prog, i), 0);
    }
}

AstNode *make_ast_node(AstNodeType type, void *inner)
{
    AstNode *node = (AstNode *)context_alloc(sizeof(AstNode));
    node->type = type;
    node->as = inner;
    return node;
}

AstNode *make_ast_lval(AstLValueKind kind)
{
    AstLValue *lval = (AstLValue *)context_alloc(sizeof(AstLValue));
    lval->kind = kind;
    return make_ast_node(AST_LVAL, lval);
}

AstNode *make_ast_unary_expr(AstNode *postfix_expr, char *value, TokenType op_type)
{
    AstUnaryExpr *unary_expr = (AstUnaryExpr *)context_alloc(sizeof(AstUnaryExpr));
    unary_expr->postfix_expr = postfix_expr;
    unary_expr->value = value;
    unary_expr->op_type = op_type;
    return make_ast_node(AST_UNARY_EXPR, unary_expr);
}

AstNode *make_expr_stmt(AstNode *expr)
{
    AstExprStmt *stmt = (AstExprStmt *)context_alloc(sizeof(AstExprStmt));
    stmt->expr = expr;
    return make_ast_node(AST_EXPR_STMT, stmt);
}

AstNode *make_ast_enum(char *value, Vector *enums)
{
    AstEnum *enm = (AstEnum *)context_alloc(sizeof(AstEnum));
    enm->enums = enums;
    enm->value = value;
    return make_ast_node(AST_ENUM, enm);
}

AstNode *make_ast_comp_stmt(Vector *body)
{
    AstCompStmt *stmt = (AstCompStmt *)context_alloc(sizeof(AstCompStmt));
    stmt->body = body;
    return make_ast_node(AST_COMP_STMT, stmt);
}

AstNode *make_int_const(int value)
{
    AstIntConst *int_const = (AstIntConst *)context_alloc(sizeof(AstIntConst));
    int_const->value = value;
    return make_ast_node(AST_INT_CONST, int_const);
}

AstNode *make_ast_bin_exp(char *value, TokenType op_type, AstNode *left, AstNode *right)
{
    AstBinExp *bin_exp = (AstBinExp *)context_alloc(sizeof(AstBinExp));
    bin_exp->left = left;
    bin_exp->right = right;
    bin_exp->value = value;
    bin_exp->op_type = op_type;
    return make_ast_node(AST_BIN_EXP, bin_exp);
}

AstNode *make_ast_func_def(char *value, char *type, AstNode *body, Vector *params)
{
    AstFuncDef *func_def = (AstFuncDef *)context_alloc(sizeof(AstFuncDef));
    func_def->body = body;
    func_def->value = value;
    func_def->type = type;
    func_def->params = params;
    return make_ast_node(AST_FUNC_DEF, func_def);
}

AstNode *make_ast_ident(char *value)
{
    AstIdent *ident = (AstIdent *)context_alloc(sizeof(AstIdent));
    ident->value = value;
    return make_ast_node(AST_IDENT, ident);
}

AstNode *make_ast_ret(AstNode *expr)
{
    AstRet *ret = (AstRet *)context_alloc(sizeof(AstRet));
    ret->expr = expr;
    return make_ast_node(AST_RET, ret);
}

AstNode *make_ast_func_call(char *value, Vector *args)
{
    AstFuncCall *func_call = (AstFuncCall *)context_alloc(sizeof(AstFuncCall));
    func_call->value = value;
    func_call->args = args;
    return make_ast_node(AST_FUNC_CALL, func_call);
}

AstNode *make_ast_var_dec(char *value, char *type)
{
    AstVarDec *dec = (AstVarDec *)context_alloc(sizeof(AstVarDec));
    dec->value = value;
    dec->type = type;
    return make_ast_node(AST_VAR_DEC, dec);
}

AstNode *make_ast_var_asgn(AstNode *lval, AstNode *rval)
{
    AstVarAsgn *asgn = (AstVarAsgn *)context_alloc(sizeof(AstVarAsgn));
    asgn->rval = rval;
    asgn->lval = lval;
    return make_ast_node(AST_VAR_ASGN, asgn);
}

AstNode *make_ast_var_def(char *value, char *type, AstNode *expr)
{
    AstVarDef *var_def = (AstVarDef *)context_alloc(sizeof(AstVarDef));
    var_def->value = value;
    var_def->type = type;
    var_def->expr = expr;
    return make_ast_node(AST_VAR_DEF, var_def);
}

AstNode *make_ast_if_else(AstNode *if_expr, AstNode *if_body, AstNode *else_body)
{
    AstIfElse *if_stmt = (AstIfElse *)context_alloc(sizeof(AstIfElse));
    if_stmt->if_body = if_body;
    if_stmt->if_expr = if_expr;
    if_stmt->else_body = else_body;
    return make_ast_node(AST_IF, if_stmt);
}

AstNode *make_ast_while(AstNode *expr, AstNode *body)
{
    AstWhile *w_stmt = (AstWhile *)context_alloc(sizeof(AstWhile));
    w_stmt->body = body;
    w_stmt->expr = expr;
    return make_ast_node(AST_WHILE, w_stmt);
}

AstNode *make_ast_for(AstNode *init, AstNode *cond, AstNode *step, AstNode *body)
{
    AstFor *f_stmt = (AstFor *)context_alloc(sizeof(AstFor));
    f_stmt->init = init;
    f_stmt->cond = cond;
    f_stmt->step = step;
    f_stmt->body = body;
    return make_ast_node(AST_FOR, f_stmt);
}
