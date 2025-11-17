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

AstNode *parse_expression(TokenStream *stream, HashMap *map);

AstNode *parse_statement(TokenStream *stream, HashMap *map);

AstNode *parse_factor(TokenStream *stream, HashMap *map);

AstNode *parse_term(TokenStream *stream, HashMap *map);

Vector *parse(Vector *tokens, HashMap *map);

AstNode *parse_var_def(TokenStream *stream, HashMap *map);

AstNode *parse_func_def(TokenStream *stream, HashMap *map);

Token *current_token(TokenStream *stream);

void next_token(TokenStream *stream);

TokenStream *make_token_stream(Vector *tokens);
#endif
