#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"

void printlvl(char* str,int level, ...){
    va_list args;
    va_start(args, level);
    printf("%*s", level, "\t");
    vprintf(str, args);
}

void _print_ast_tree(AstNode *node, int level){
    switch(node->type){
        case AST_FUNC_DEF:
            AstFuncDef * func_def = (AstFuncDef*)node->as;
            printlvl("FuncDef(\n", level);

            printlvl("\tname='%s'\n", level, func_def->value);
            printlvl("\tparams=[\n", level, func_def->params);
            for(int i = 0; i< func_def->params->length; i++){
                printlvl("\t\t'%s'\n",level, (char*)vector_get(func_def->params, i));
            }
            printlvl("\t\t]\n", level);

            printlvl("\tbody=[\n", level);
            _print_ast_tree(func_def->body, level + 1);

            printlvl("\t\t]\n", level);
            printlvl(")\n", level);
            break;
        case AST_VAR_DEF:
            AstVarDef *var_def = (AstVarDef*)node->as;
            printlvl("VarDef(\n", level);

            printlvl("\tname='%s'\n", level,var_def->value);

            printlvl("\tvalue=(\n",level);
            _print_ast_tree(var_def->expr, level+1);
            printlvl(")\n", level);
            break;
        case AST_BIN_EXP:
            AstBinExp *bin_exp = (AstBinExp*)node->as;
            printlvl("BinExp(\n",level);
            printlvl("\top=%c\n", level, *(bin_exp->value));

            printlvl("\tleft=(\n",level);
            _print_ast_tree(bin_exp->left, level + 1);
            printlvl("\t)\n",level);

            printlvl("\tright=(\n",level);
            _print_ast_tree(bin_exp->right, level + 1);
            printlvl("\t)\n",level);

            printlvl(")\n", level);
            break;

        case AST_INT_CONST:
            AstIntConst *cons = (AstIntConst*)node->as;
            int lit;
            str2int(&lit, cons->value, 10);
            printlvl("IntConst(%d)\n", level, lit);
            break;
        case AST_STATEMENT:
            AstStatement *stmt = (AstStatement*)node->as;
            for(int i=0; i < stmt->body->length; i++){
                _print_ast_tree((AstNode*)vector_get(stmt->body, i), level + 1);
            }
            break;
        default:
            printf("type%d", node->type);
            perror("not impl");

    }
}

void print_ast_tree(AstNode *node)
{
    _print_ast_tree(node, 0);
}

void print_ast_node(AstNode *node)
{
    // printf("<AST NODE TYPE=%d VALUE=%s >\n", node->type, node->value);
}

AstNode *make_ast_node(AstNodeType type, void *inner)
{
    AstNode *node = (AstNode *)my_malloc(sizeof(AstNode));
    node->type = type;
    node->as = inner;
    return node;
}

AstNode *make_ast_stmt(Vector *body)
{
    AstStatement *stmt = (AstStatement *)my_malloc(sizeof(AstStatement));
    stmt->body = body;
    return make_ast_node(AST_STATEMENT, stmt);
}

AstNode *make_int_const(char *value)
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

AstNode *make_ast_func_def(char *value, AstNode *body, Vector *params)
{
    AstFuncDef *func_def = (AstFuncDef *)my_malloc(sizeof(AstFuncDef));
    func_def->body = body;
    func_def->value = value;
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

AstNode *make_ast_var_def(char *value, AstNode *expr)
{
    AstVarDef *var_def = (AstVarDef *)my_malloc(sizeof(AstVarDef));
    var_def->value = value;
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

void _ast_free(AstNode *node)
{
}

// void ast_free(AstNode* node){
//     if(node == NULL){
//         return  ;
//     }
//     if(node->body){
//         for(int i=0 ; i < node->body->length ; i++){
//             ast_free(vector_get(node->body,  i));
//         }
//         //Free Vector manually
//         // Since nodes are free'd
//         free(node->body->array);
//         free(node->body);
//     }

//     if(node->args){
//         //Free Vector manually
//         // Values cleaned up by symbol table
//         free(node->args->array);
//         free(node->args);
//     }
//     ast_free(node->left);
//     ast_free(node->right);

//     free(node);
// }