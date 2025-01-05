#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "util.h"


void print_ast_node(AstNode* node){
    printf("<AST NODE TYPE=%d VALUE=%s >\n", node->type, node->value);
}


AstNode* make_ast_node(int type, char* value, AstNode* left, AstNode* right, Vector* body){
  AstNode* n;
  n = my_malloc(sizeof(AstNode));

  n->type = type;
  n->value =value;
  n->left = left;
  n->right = right;
  n->body = body;
  return n;
}

int ast_eval(AstNode* node){
    if( node == NULL){
        return 0;
    }
    if(node->type == LITERAL){
        int out;
        str2int(&out, node->value,10);
        return out;
    }


    int left = ast_eval(node->left);
    int right = ast_eval(node->right);
    switch (*(node->value)) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            return left / right;
    }
}
