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
}

void ast_free(AstNode* node){
    if(node == NULL){
        return  ;
    }
    if(node->body){
        for(int i=0 ; i < node->body->length ; i++){
            ast_free(vector_get(node->body,  i));
        }
        //Free Vector manually
        // Since nodes are free'd
        free(node->body->array);
        free(node->body);
    }

    if(node->args){
        //Free Vector manually 
        // Values cleaned up by symbol table
        free(node->args->array);
        free(node->args);
    }
    ast_free(node->left);
    ast_free(node->right);

    free(node);
}