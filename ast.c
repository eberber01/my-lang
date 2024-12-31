#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "util.h"


void print_ast_node(struct AstNode* node){
    printf("<AST NODE TYPE=%d VALUE=%s >\n", node->type, node->value);
}


struct AstNode* make_ast_node(int type, char* value, struct AstNode* left, struct AstNode* right){
  struct AstNode* n;
  n = my_malloc(sizeof(AstNode));

  n->type = type;
  n->value =value;
  n->left = left;
  n->right = right;
  return n;
}

int ast_eval(struct AstNode* node){
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
