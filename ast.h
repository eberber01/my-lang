#ifndef AST_H
#define AST_H

enum NodeType { 
        LITERAL, 
        IDENTIFIER, 
        BINARY_EXPR 
};


typedef struct AstNode{
    int type;
    struct AstNode* left;
    struct AstNode* right;
    char* value;
} AstNode;

void visit(struct AstNode* node);

int ast_eval(struct AstNode* node);
struct AstNode* make_ast_node(int type,char* value, struct AstNode* left, struct AstNode* right);


#endif