#ifndef PARSE_H
#define PARSE_H
#include "lex.h"
#include "ast.h"

typedef struct TokenStream{
    struct Token* tokens;
    size_t current;
    size_t size;
} TokenStream;

AstNode* parse_expression(TokenStream* stream);
AstNode* parse_factor(TokenStream* stream);
AstNode* parse_term(TokenStream* stream);
AstNode* parse(TokenStream* stream);
AstNode* parse_variable(TokenStream* stream);



Token* current_token(TokenStream* stream);
void next_token(TokenStream* stream);
TokenStream* make_token_stream(Token* tokens, size_t size);

#endif