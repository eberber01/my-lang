#ifndef AST_H
#define AST_H
#include "util.h"
typedef enum NodeType { 
        LITERAL, 
        IDENTIFIER, 
        BINARY_EXPR,
        FUNC_DECLARE, 
        VAR_DECLARE,
}NodeType;


typedef struct AstNode{
    int type;
    struct AstNode* left;
    struct AstNode* right;
    char* value;
    Vector* body;
} AstNode;

void visit(AstNode* node);

int ast_eval(AstNode* node);
struct AstNode* make_ast_node(int type,char* value,AstNode* left, AstNode* right, Vector* body);


#endif