#ifndef AST_H
#define AST_H
#include "util.h"
typedef enum NodeType { 
        AST_LITERAL, 
        AST_IDENTIFIER, 
        AST_BINARY_EXPR,
        AST_FUNC_DEF, 
        AST_FUNC_DEC,
        AST_VAR_DEF,
        AST_VAR_DEC,
        AST_VAR,
        AST_STATEMENT,
        AST_FUNC_CALL,
        AST_RET
}NodeType;


typedef struct AstNode{
    int type;
    struct AstNode* left;
    struct AstNode* right;
    char* value;
    Vector* body;
    Vector* args;
} AstNode;

void visit(AstNode* node);
void ast_free(AstNode* root);
int ast_eval(AstNode* node);
struct AstNode* make_ast_node(int type,char* value,AstNode* left, AstNode* right, Vector* body, Vector* args);


#endif