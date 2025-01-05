#ifndef PARSE_H
#define PARSE_H
#include "lex.h"
#include "ast.h"
#include "util.h"
#include "symtab.h"

typedef struct TokenStream{
    Vector* tokens;
    size_t current;
} TokenStream;

AstNode* parse_expression(TokenStream* stream, SymTab* table);
AstNode* parse_factor(TokenStream* stream, SymTab* table);
AstNode* parse_term(TokenStream* stream, SymTab* table);
AstNode* parse(Vector* tokens, SymTab* table);
AstNode* parse_variable(TokenStream* stream, SymTab* table);



Token* current_token(TokenStream* stream);
void next_token(TokenStream* stream);
TokenStream* make_token_stream(Vector* tokens);

#endif