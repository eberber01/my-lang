//#include "ast.h"
#include "ast.h"
#include "lex.h"
#include "symtab.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include "parse.h"


Token* current_token(TokenStream* stream){
    if(stream->current >= stream->tokens->length){
        return NULL;
    }
    return (Token*)vector_get(stream->tokens,  stream->current);
}

Token* peek(TokenStream* stream, int n){
    if(n + stream->current >= stream->tokens->length){
        return NULL;
    }
    return (Token*)vector_get(stream->tokens, n + stream->current);

}

void next_token(TokenStream* stream){
    if(stream->current >= stream->tokens->length){
        return;
    }
    (stream->current)++;
}

TokenStream* make_token_stream(Vector* tokens){
    TokenStream* stream = my_malloc(sizeof(TokenStream));

    stream->tokens = tokens;
    stream->current = 0;
    return stream;
}


Token* expect(TokenStream* stream, TokenType expect){
    Token* current = current_token(stream);
    if(current == NULL){
        return current;
    }
    if(current->type == expect){
        next_token(stream);
        return current;
    }
    printf("Expected token: %d", expect);
    exit(1);
}


AstNode* parse_expression(TokenStream* stream, SymTab* table){

    AstNode* left= parse_term(stream, table);
    Token* current;
    while((current = current_token(stream)) && (current->type == ADD || current->type == SUB)){
        char* value = current->value;
        next_token(stream);
        AstNode* right = parse_term(stream, table);
        left = make_ast_node(BINARY_EXPR, value, left, right, NULL);
    }
    return left;

}

AstNode* parse_term(TokenStream* stream, SymTab* table){
    AstNode* left = parse_factor(stream, table);
    Token* current;
    while((current = current_token(stream)) && (current->type == DIV || current->type == MULT)){
        char* value = current->value;
        next_token(stream);
        AstNode* right = parse_factor(stream, table);
        left = make_ast_node(BINARY_EXPR,value , left, right, NULL);
    }
    return left;

}

AstNode* parse_factor(TokenStream* stream, SymTab* table){
    Token* current = current_token(stream); 
    if(current->type == NUM){
        next_token(stream);
        return make_ast_node(LITERAL, current->value, NULL, NULL, NULL);
    }
    else if(current->type == IDENT){
        if(symtab_get(table,  current->value) == NULL){
            perror("Cannot used undefined variable");
            exit(1);
        }
        next_token(stream);
        return make_ast_node(VAR, current->value, NULL, NULL, NULL);

    }
    else if( current->type == LPAREN){
        next_token(stream);
        AstNode* ret =  parse_expression(stream, table);

        //TODO: account for paren
        expect(stream, RPAREN);

        return ret;
    }
    return NULL;
}

int is_func_dec_start(TokenStream* stream){

    Token* func_type = current_token(stream);
    Token* func_name = peek(stream, 1);
    Token* paren = peek(stream, 2);
    return func_type->type == TYPE && func_name->type == IDENT && paren->type == LPAREN;
}

int is_var_dec_start(TokenStream* stream){

    Token* var_type = current_token(stream);
    Token* var_name = peek(stream, 1);
    Token* assign = peek(stream, 2);
    return var_type->type == TYPE && var_name->type == IDENT && assign->type == ASSIGN;
}

AstNode* parse_statement(TokenStream* stream, SymTab* table) {
    Token* current;
    Vector* body = vector_new(); 
    while((current = current_token(stream)) && current_token(stream)-> type != RCBRACKET){
        print_token(current_token(stream));
        if (is_func_dec_start(stream)) {
            // Variable declaration/assignment
            AstNode* func = parse_function( stream,  table);
            vector_push(body, func);
        }
        else if (is_var_dec_start(stream)) {
            // Variable declaration/assignment
            AstNode* var = parse_variable( stream,  table);
            vector_push(body,  var);
        }
        else{
            perror("Syntax Error");
            exit(1);
        }
    }
    return make_ast_node(STATEMENT, "STATEMENT", NULL,  NULL, body);
}

Vector* parse_body(TokenStream* stream, SymTab* table) {
    Vector* body = vector_new();
    while (current_token(stream) && current_token(stream)->type != RCBRACKET) {
        AstNode* stmt = parse_statement(stream, table);
        vector_push(body, stmt);
    }
    return body;
}

Vector* parse_func_args(TokenStream *stream, SymTab *table){
    Vector* args = vector_new();
    Token* current;
    while((current = current_token(stream)) && current_token(stream)->type == TYPE) {
        Token* type = expect(stream, TYPE);

        //argument name
        expect(stream, IDENT);
        if(current_token(stream)->type == RPAREN){
           break; 
        }
        expect(stream, COMMA);

        vector_push(args,  type->value);
    }

    return args;
}


AstNode* parse_function(TokenStream* stream, SymTab* table){
    //Get return type
    Token* func_type = expect(stream, TYPE);

    char* func_name = expect(stream, IDENT)->value;
    // Check if symbol exists
    if(symtab_get(table,  func_name)){
        perror("Redefinition of function.");
        exit(1);
    }

    // TODO: Parse args here
    expect(stream, LPAREN);
    Vector* args = parse_func_args( stream,  table);
    expect(stream, RPAREN);

    //Insert args into symbol table
    // TODO: Make Symbol type to replace
    symtab_add(table, make_symtab_entry( func_name,  symtab_get(table, func_type->value)->type,  FUNCTION, args));

    // start of function
    expect(stream, LCBRACKET);
    //Ast list
    Vector* body = parse_body(stream,  table); // Parse function body

    expect(stream, RCBRACKET);
    return make_ast_node(FUNC_DEF, func_name, NULL, NULL ,  body);
}

AstNode* parse_variable(TokenStream* stream, SymTab* table){
    // get type
    Token* var_type = expect(stream, TYPE);
    // get variable name
    char* name = expect(stream, IDENT)->value;

    // Check if symbol exists
    if(symtab_get(table,  name)){
        perror("Redefinition of variable.");
        exit(1);
    }

    AstNode* init = NULL;
    if(current_token(stream)->type == ASSIGN){
        next_token(stream);
        init =  parse_expression(stream, table);
    }

    expect(stream, SEMICOLON);

    //Successfull parse
    symtab_add(table, make_symtab_entry(name, symtab_get(table,  var_type->value)->type, VARIABLE, NULL));
    return make_ast_node(VAR_DEF, name, init, NULL, NULL ); 
}

AstNode* parse(Vector* tokens, SymTab* table){
    TokenStream * stream = make_token_stream(tokens);
    AstNode* root = parse_statement(stream, table);
    free(stream);
    return root;
}
