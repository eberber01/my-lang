#ifndef PARSE_H
#define PARSE_H
#include "ast.h"
#include "hashmap.h"
#include "lex.h"
#include "util.h"

typedef struct TokenStream
{
    Vector *tokens;
    size_t current;
} TokenStream;

AstNode *parse_expression(TokenStream *stream);

AstNode *parse_statement(TokenStream *stream);

AstNode *parse_factor(TokenStream *stream);

AstNode *parse_term(TokenStream *stream);

Vector *parse(Vector *tokens);

AstNode *parse_var_def(TokenStream *stream);

AstNode *parse_func_def(TokenStream *stream);

Token *current_token(TokenStream *stream);

AstNode *parse_var_dec(TokenStream *stream);

AstNode *parse_var_asgn(TokenStream *stream);

void next_token(TokenStream *stream);

TokenStream *make_token_stream(Vector *tokens);
#endif
