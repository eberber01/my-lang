#ifndef AST_H
#define AST_H
#include "hashmap.h"
#include "lex.h"
#include "util.h"

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
    AST_BOOL_EXPR,
    AST_ENUM,
    AST_VAR_DEC,
    AST_VAR_ASGN,
    AST_WHILE,
    AST_FOR,
    AST_EMPTY_EXPR,
    AST_EXPR_STMT,
    AST_UNARY_EXPR
} AstNodeType;

typedef struct Param
{
    char *value;
    char *type;
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
    TokenType op_type;
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
    SymTabEntry *func;
} AstRet;

typedef struct AstFuncCall
{
    char *value;
    Vector *args;
    SymTabEntry *symbol;
} AstFuncCall;

typedef struct AstVarDef
{
    char *value;
    char *type;
    AstNode *expr;
    SymTabEntry *symbol;
} AstVarDef;

typedef struct AstVarDec
{
    char *value;
    char *type;
    SymTabEntry *symbol;
} AstVarDec;

typedef struct AstVarAsgn
{
    char *value;
    AstNode *expr;
    SymTabEntry *symbol;
} AstVarAsgn;

typedef struct AstIfElse
{
    AstNode *if_expr;
    AstNode *if_body;
    AstNode *else_body;
} AstIfElse;

typedef struct AstEnum
{
    char *value;
    Vector *enums;
} AstEnum;

typedef struct AstWhile
{
    AstNode *expr;
    AstNode *body;
} AstWhile;

typedef struct AstFor
{
    AstNode *init;
    AstNode *cond;
    AstNode *step;
    AstNode *body;
} AstFor;

typedef struct AstExprStmt
{
    AstNode *expr;
} AstExprStmt;

typedef struct AstUnaryExpr
{
    AstNode *postfix_expr;
    char *value;
    TokenType op_type;
} AstUnaryExpr;

void print_ast(Vector *tree);
void visit(AstNode *node);

void ast_free(AstNode *root);

int ast_eval(AstNode *node);

struct AstNode *make_ast_node(AstNodeType type, void *inner);

AstNode *make_ast_comp_stmt(Vector *body);

AstNode *make_int_const(int value);

AstNode *make_expr_stmt(AstNode *expr);

AstNode *make_ast_bin_exp(char *value, TokenType op_type, AstNode *left, AstNode *right);

AstNode *make_ast_func_def(char *value, char *type, AstNode *body, Vector *params);

AstNode *make_ast_ident(char *value);

AstNode *make_ast_ret(AstNode *expr);

AstNode *make_ast_func_call(char *value, Vector *args);

AstNode *make_ast_var_def(char *value, char *type, AstNode *expr);

AstNode *make_ast_bool_expr(char *value, AstNode *left, AstNode *right);

AstNode *make_ast_if_else(AstNode *if_expr, AstNode *if_body, AstNode *else_body);

AstNode *make_ast_enum(char *value, Vector *enums);

AstNode *make_ast_var_dec(char *value, char *type);

AstNode *make_ast_var_asgn(char *value, AstNode *expr);

AstNode *make_ast_while(AstNode *expr, AstNode *body);

AstNode *make_ast_for(AstNode *init, AstNode *cond, AstNode *step, AstNode *body);

AstNode *make_ast_unary_expr(AstNode *postfix_expr, char *value, TokenType op_type);

void _print_ast(AstNode *, int);

#endif
