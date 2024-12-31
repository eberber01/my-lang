//#include "ast.h"
#include "ast.h"
#include "lex.h"
#include <stdio.h>
#include "parse.h"

AstNode* parse_expression(Token* tokens, int token_len, int* current_token){

    AstNode* left= parse_term(tokens, token_len, current_token);
    while(tokens[*current_token].type == ADD || tokens[*current_token].type == SUB){
        char* value = tokens[*current_token].value;
        (*current_token)++;
        AstNode* right = parse_term( tokens,token_len,current_token);
        left = make_ast_node(BINARY_EXPR, value, left, right);
    }
    return left;

}

AstNode* parse_term(Token* tokens, int token_len, int* current_token){

    AstNode* left = parse_factor(tokens, token_len, current_token);
    while(tokens[*current_token].type == DIV || tokens[*current_token].type == MULT){
        char* value = tokens[*current_token].value;
        (*current_token)++;
        AstNode* right = parse_factor(tokens,token_len,current_token);
        left = make_ast_node(BINARY_EXPR,value , left, right);
    }
    return left;

}

AstNode* parse_factor(Token* tokens, int token_len, int* current_token){
    if(tokens[*current_token].type == NUM){
        char* value = tokens[*current_token].value;
        (*current_token)++;
        return make_ast_node(LITERAL, value, NULL, NULL);
    }
    else if( tokens[*current_token].type == LPAREN){
        (*current_token)++;
        AstNode* ret =  parse_expression( tokens, token_len, current_token);
        //TODO: account for paren
        if(tokens[*current_token].type != RPAREN){
            //syntax error
            printf("MISSING RPAREN");
        }
        (*current_token)++;

        return ret;
    }
    return NULL;
}

AstNode* parse(Token* tokens, int token_len){
    int current_token = 0;
    return parse_expression(tokens, token_len,&current_token);
}
