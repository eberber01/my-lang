//#include "ast.h"
#include "ast.h"
#include "lex.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include "parse.h"


Token* current_token(TokenStream* stream){
    return stream->tokens + stream->current;
}
void next_token(TokenStream* stream){
    if(stream->current >= stream->size){
        return;
    }
    (stream->current)++;
}

TokenStream* make_token_stream(Token* tokens, size_t size){
    TokenStream* stream = my_malloc(sizeof(TokenStream));

    stream->tokens = tokens;
    stream->size = size;
    stream->current = 0;
    return stream;
}


Token* expect(TokenStream* stream, TokenType expect){
    Token* current = current_token(stream);
    if(current->type == expect){
        next_token(stream);
        return current;
    }
    printf("Expected token: %d", expect);
    exit(1);
}


AstNode* parse_expression(TokenStream* stream){

    AstNode* left= parse_term(stream);
    Token* current = current_token(stream);
    while(current->type == ADD || current->type == SUB){
        char* value = current->value;
        next_token(stream);
        AstNode* right = parse_term(stream);
        left = make_ast_node(BINARY_EXPR, value, left, right);
    }
    return left;

}

AstNode* parse_term(TokenStream* stream){
    AstNode* left = parse_factor(stream);
    Token* current = current_token(stream);
    while(current->type == DIV || current->type == MULT){
        char* value = current->value;
        next_token(stream);
        AstNode* right = parse_factor(stream);
        left = make_ast_node(BINARY_EXPR,value , left, right);
    }
    return left;

}

AstNode* parse_factor(TokenStream* stream){
    Token* current = current_token(stream); 
    if(current->type == NUM){
        char* value = current_token(stream)->value;
        next_token(stream);
        return make_ast_node(LITERAL, value, NULL, NULL);
    }
    else if( current->type == LPAREN){
        next_token(stream);
        AstNode* ret =  parse_expression(stream);

        //TODO: account for paren
        expect(stream, RPAREN);

        return ret;
    }
    return NULL;
}

AstNode* parse_statement(TokenStream* stream) {
    Token* current = current_token(stream);

    if (current->type == TYPE) {
        // Variable declaration/assignment
        return parse_variable(stream);
    }
    else {
        // Default to parsing an expression
        AstNode* expr = parse_expression(stream);
        expect(stream, SEMICOLON);
        return expr;
    }
}

List* parse_body(TokenStream* stream) {
    List* body = create_list();
    while (current_token(stream)->type != RCBRACKET) {
        AstNode* stmt = parse_statement(stream);
        add_to_list(body, stmt);
    }
    return body;
}



AstNode* parse_function(TokenStream* stream){
    //Get return type
    printf("PARSE\n");
    print_token(*current_token(stream));
    Token* func_type = expect(stream, TYPE);

    Token* func_name = expect(stream, IDENT);

    // TODO: Parse args here
    expect(stream, LPAREN);
    expect(stream, RPAREN);
    // start of function

    expect(stream, LCBRACKET);
    List* body = parse_body(stream); // Parse function body

    expect(stream, RCBRACKET);

    return make_ast_node(FUNC_DECLARE, func_name->value, body);
}

AstNode* parse_variable(TokenStream* stream){
    // get type
    TokenType type = expect(stream, TYPE)->type;
    // get variable name
    char* name = expect(stream, IDENT)->value;
    AstNode* init = NULL;
    if(current_token(stream)->type == ASSIGN){
        next_token(stream);
        init =  parse_expression(stream);
    }

    // expect semicolon
    expect(stream, SEMICOLON);
    return make_ast_node(VAR_DECLARE, name, init, NULL); 
}

AstNode* parse(TokenStream* stream){
    return parse_function(stream);
}
