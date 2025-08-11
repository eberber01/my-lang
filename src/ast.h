#ifndef AST_H
#define AST_H
#include "util.h"
#include "symtab.h"


typedef enum AstNodeType
{
    AST_INT_CONST,
    AST_BIN_EXP,
    AST_FUNC_DEF,
    AST_VAR_DEF,
    AST_IDENT,
    AST_COMP_STMT,
    AST_FUNC_CALL,
    AST_RET,
    AST_IF,
    AST_BOOL_EXPR
} AstNodeType;

typedef struct Param{
    char* value;
    char* type;
    SymTabEntry *symbol;
} Param;

typedef struct AstNode
{
    AstNodeType type;
    void *as;
} AstNode;

typedef struct AstCompStmt
{
    // List of AstNode Statements
    Vector *body;
} AstCompStmt;

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
    char *type;
    AstNode *body;
    Vector *params;
    SymTabEntry *symbol;
} AstFuncDef;

typedef struct AstIdent
{
    char *value;

    SymTabEntry *symbol;
} AstIdent;

typedef struct AstRet
{
    AstNode *expr;
    char* func;
} AstRet;

typedef struct AstFuncCall
{
    char *value;
    Vector *args;
} AstFuncCall;

typedef struct AstVarDef
{
    char *value;
    char *type;
    AstNode *expr;
    SymTabEntry *symbol;
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
    AstNode *body;
} AstIf;

void visit(AstNode *node);

void ast_free(AstNode *root);

int ast_eval(AstNode *node);

struct AstNode *make_ast_node(AstNodeType type, void *inner);

AstNode *make_ast_comp_stmt(Vector *body);

AstNode *make_int_const(int value);

AstNode *make_ast_bin_exp(char *value, AstNode *left, AstNode *right);

AstNode *make_ast_func_def(char *value, char* type, AstNode *body, Vector *params);

AstNode *make_ast_ident(char *value);

AstNode *make_ast_ret(AstNode *expr);

AstNode *make_ast_func_call(char *value, Vector *args);

AstNode *make_ast_var_def(char *value, char* type, AstNode *expr);

AstNode *make_ast_bool_expr(char *value, AstNode *left, AstNode *right);

AstNode *make_ast_if(AstNode *expr, AstNode *body);

#endif
