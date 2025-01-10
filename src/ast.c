#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "util.h"

void print_ast_node(AstNode* node){
    printf("<AST NODE TYPE=%d VALUE=%s >\n", node->type, node->value);
}

AstNode* make_ast_node(int type, char* value, AstNode* left, AstNode* right, Vector* body, Vector* args){
  AstNode* n;
  n = my_malloc(sizeof(AstNode));

  n->type = type;
  n->value =value;
  n->left = left;
  n->right = right;
  n->body = body;
  n->args = args;
  return n;
}

void _ast_free(AstNode* node){
    if(node == NULL){
        return  ;
    }
    _ast_free(node->left);
    _ast_free(node->right);


    free(node);
    free(node->body);
}

void ast_free(AstNode* root){
    if(root->body){
        for(int i=0 ; i < root->body->length ; i++){
            _ast_free(vector_get(root->body,  i));
        }
    }
}