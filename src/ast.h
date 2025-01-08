#ifndef AST_H
#define AST_H
#include "util.h"
typedef enum NodeType { 
        LITERAL, 
        IDENTIFIER, 
        BINARY_EXPR,
        FUNC_DEF, 
        FUNC_DEC,
        VAR_DEF,
        VAR_DEC,
        VAR
}NodeType;


typedef struct AstNode{
    int type;
    struct AstNode* left;
    struct AstNode* right;
    char* value;
    Vector* body;
} AstNode;

void visit(AstNode* node);
void ast_free(AstNode* root);
int ast_eval(AstNode* node);
struct AstNode* make_ast_node(int type,char* value,AstNode* left, AstNode* right, Vector* body);


#endif