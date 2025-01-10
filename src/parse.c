//#include "ast.h"
#include "ast.h"
#include "lex.h"
#include "symtab.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "parse.h"

//Get current token in stream 
//Returns NULL if end of stream
Token* current_token(TokenStream* stream){
    if(stream->current >= stream->tokens->length  ){
        return NULL;
    }
    return (Token*)vector_get(stream->tokens,  stream->current);
}

//Get next nth token in stream
//Returns NULL if outside of stream size
Token* peek(TokenStream* stream, int n){
    if(n + stream->current >= stream->tokens->length){
        return NULL;
    }
    return (Token*)vector_get(stream->tokens, n + stream->current);

}

//Set next token in stream
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

// Returns current token in stream 
// if TokenType matches 'expect'
// errors otherwise
Token* expect(TokenStream* stream, TokenType expect){
    Token* current = current_token(stream);
    if(current == NULL){
        return current;
    }
    if(current->type == expect){
        next_token(stream);
        return current;
    }
    exit(1);
}


AstNode* parse_expression(TokenStream* stream, SymTab* table){

    AstNode* left= parse_term(stream, table);
    Token* current;
    while((current = current_token(stream)) && (current->type == ADD || current->type == SUB)){
        char* value = current->value;
        next_token(stream);
        AstNode* right = parse_term(stream, table);
        left = make_ast_node(AST_BINARY_EXPR, value, left, right, NULL, NULL);
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
        left = make_ast_node(AST_BINARY_EXPR,value , left, right, NULL, NULL);
    }
    return left;
}

int is_func_call_start(TokenStream* stream, SymTab* table){
    Token* func_name = current_token(stream);
    Token* lparen = peek(stream,  1);
    return func_name->type == IDENT && lparen->type == LPAREN; 
}

AstNode* parse_func_call(TokenStream* stream, SymTab* table){
    Token* name = expect(stream,  IDENT);
    //TODO:CHECK IF DEFINED IN SYMBOL TABLE

    expect(stream,  LPAREN);

    Vector* args = vector_new(); 
    while(current_token(stream)->type != RPAREN){
        //TODO Double check arg types
        vector_push(args, expect(stream,  IDENT)->value);
        expect(stream,  COMMA);
    }

    expect(stream,  RPAREN);
    return make_ast_node(AST_FUNC_CALL, name->value , NULL, NULL, NULL, args);
}

AstNode* parse_factor(TokenStream* stream, SymTab* table){
    Token* current = current_token(stream); 
    if(current->type == NUM){
        next_token(stream);
        return make_ast_node(AST_LITERAL, current->value, NULL, NULL, NULL, NULL);
    }
    else if(current->type == IDENT){
        if(is_func_call_start(stream,  table)){
            return parse_func_call(stream, table);
        }

        if(symtab_get(table,  current->value) == NULL){
            perror("Cannot used undefined variable");
            exit(1);
        }
        next_token(stream);
        return make_ast_node(AST_VAR, current->value, NULL, NULL, NULL, NULL);

    }
    else if( current->type == LPAREN){
        next_token(stream);
        AstNode* ret =  parse_expression(stream, table);

        expect(stream, RPAREN);

        return ret;
    }
    else{
        return NULL;
    }
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


AstNode* parse_boolean_expression(TokenStream* stream, SymTab* table){
    Token* current;
    AstNode* left = parse_expression(stream, table);
    while((current = current_token(stream)) && (current->type == EQUAL)){
        char* value = current->value;
        next_token(stream);
        AstNode* right = parse_expression(stream, table);
        left = make_ast_node(AST_BOOL_EXPR, value, left, right, NULL, NULL);
    }
    return left;
}

AstNode* parse_if_statement(TokenStream* stream, SymTab* table){
    expect(stream, IF);
    expect(stream, LPAREN);
    AstNode*  expr = parse_boolean_expression(stream, table);

    expect(stream, RPAREN);
    expect(stream, LBRACE);

    Vector* if_body = parse_body(stream, table);

    expect(stream, RBRACE);
    return  make_ast_node(AST_IF,  "if", expr, NULL, if_body, NULL);
}

AstNode* parse_statement(TokenStream* stream, SymTab* table) {
    Token* current;
    Vector* body = vector_new(); 
    while( (current = current_token(stream)) && current_token(stream)-> type != RBRACE){
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
        else if(current->type == RETURN){
            expect(stream, RETURN);
            AstNode* expr = parse_expression(stream, table);

            AstNode* ret_node = make_ast_node(AST_RET,  "ret", expr, NULL, NULL, NULL);
            vector_push(body,  ret_node);
            expect(stream, SEMICOLON);
        }
        else if(current->type == IF){
            AstNode* if_state = parse_if_statement(stream, table);
            vector_push(body,  if_state);
        
        }
        else{
            AstNode* expr = parse_expression( stream,  table);
            vector_push(body,  expr);
            expect(stream,  SEMICOLON);
        }
    }
    return make_ast_node(AST_STATEMENT, "STATEMENT", NULL,  NULL, body, NULL);
}

Vector* parse_body(TokenStream* stream, SymTab* table) {
    Vector* body = vector_new();
    while (current_token(stream) && current_token(stream)->type != RBRACE) {
        AstNode* stmt = parse_statement(stream, table);
        vector_push(body, stmt);
    }
    return body;
}

Vector* parse_func_params(TokenStream *stream, SymTab *table){
    Vector* params = vector_new();
    Token* current;
    while((current = current_token(stream)) && current_token(stream)->type == TYPE) {
        Token* param_type = expect(stream, TYPE);
        expect(stream, IDENT);
        if(current_token(stream)->type == RPAREN){
           break; 
        }
        expect(stream, COMMA);

        vector_push(params,  param_type->value);
    }

    return params;
}

AstNode* parse_function(TokenStream* stream, SymTab* table){
    Token* func_type = expect(stream, TYPE);
    Token* func_name = expect(stream, IDENT);

    // Check if symbol exists in table
    if(symtab_get(table,  func_name->value)){
        perror("Redefinition of function.");
        exit(1);
    }

    //Parse function args
    expect(stream, LPAREN);
    Vector* params = parse_func_params( stream,  table);
    expect(stream, RPAREN);

    //Insert args into symbol table
    Type ret_type = symtab_get(table, func_type->value)->type;
    SymTabEntry* entry = make_symtab_entry( func_name->value,  ret_type,  FUNCTION, params);
    symtab_add(table, entry);

    //Parse function body
    expect(stream, LBRACE);
    Vector* body = parse_body(stream,  table);
    expect(stream, RBRACE);

    return make_ast_node(AST_FUNC_DEF, func_name->value, NULL, NULL ,  body, NULL);
}

AstNode* parse_variable(TokenStream* stream, SymTab* table){
    Token* var_type = expect(stream, TYPE);
    Token* var_name = expect(stream, IDENT);

    // Check if symbol exists in table
    if(symtab_get(table,  var_name->value)){
        perror("Redefinition of variable.");
        exit(1);
    }

    //Parse init expression
    AstNode* init = NULL;
    if(current_token(stream)->type == ASSIGN){
        next_token(stream);
        init =  parse_expression(stream, table);
    }

    expect(stream, SEMICOLON);

    //Successfull parse add to table
    SymTabEntry* entry = make_symtab_entry(var_name->value, symtab_get(table,  var_type->value)->type, VARIABLE, NULL);
    symtab_add(table, entry);

    return make_ast_node(AST_VAR_DEF, var_name->value, init, NULL, NULL, NULL ); 
}

//Returns root AstNode representing program
AstNode* parse(Vector* tokens, SymTab* table){
    TokenStream * stream = make_token_stream(tokens);
    AstNode* root = parse_statement(stream, table);
    free(stream);
    return root;
}
