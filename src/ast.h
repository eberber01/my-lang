#ifndef AST_H
#define AST_H
#include "util.h"
typedef enum AstNodeType
{
    AST_INT_CONST,
    AST_IDENTIFIER,
    AST_BIN_EXP,
    AST_FUNC_DEF,
    AST_FUNC_DEC,
    AST_VAR_DEF,
    AST_VAR_DEC,
    AST_IDENT,
    AST_STATEMENT,
    AST_FUNC_CALL,
    AST_RET,
    AST_IF,
    AST_BOOL_EXPR
} AstNodeType;

typedef struct AstNode
{
    AstNodeType type;
    void *as;
} AstNode;

typedef struct AstStatement
{
    // List of AstNode Statements
    Vector *body;
} AstStatement;

typedef struct AstIntConst
{
    int value;
} AstIntConst;

typedef struct AstBinExp
{
    char *value;
    struct AstNode *left;
    struct AstNode *right;
} AstBinExp;

typedef struct AstFuncDef
{
    char *value;
    Vector *body;
    Vector *params;
} AstFuncDef;

typedef struct AstIdent
{
    char *value;
} AstIdent;

typedef struct AstRet
{
    AstNode *expr;
} AstRet;

typedef struct AstFuncCall
{
    char *value;
    Vector *args;
} AstFuncCall;

typedef struct AstVarDef
{
    char *value;
    AstNode *expr;
} AstVarDef;

typedef struct AstBoolExpr
{
    char *value;
    AstNode *left;
    AstNode *right;
} AstBoolExpr;

typedef struct AstIf
{
    AstNode *expr;
    Vector *body;
} AstIf;

void visit(AstNode *node);
void ast_free(AstNode *root);
int ast_eval(AstNode *node);
struct AstNode *make_ast_node(AstNodeType type, void *inner);

AstNode *make_ast_stmt(Vector *body);
AstNode *make_int_const(int value);
AstNode *make_ast_bin_exp(char *value, AstNode *left, AstNode *right);
AstNode *make_ast_func_def(char *value, Vector *body, Vector *params);
AstNode *make_ast_ident(char *value);
AstNode *make_ast_ret(AstNode *expr);
AstNode *make_ast_func_call(char *value, Vector *args);
AstNode *make_ast_var_def(char *value, AstNode *expr);
AstNode *make_ast_bool_expr(char *value, AstNode *left, AstNode *right);
AstNode *make_ast_if(AstNode *expr, Vector *body);

#endif