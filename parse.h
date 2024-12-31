#ifndef PARSE_H
#define PARSE_H
#include "lex.h"

struct AstNode* parse_expression(Token* tokens, int token_len, int* current_token);
struct AstNode* parse_factor(Token* tokens, int token_len, int* current_token);
struct AstNode* parse_term(Token* tokens, int token_len, int* current_token);
struct AstNode* parse(Token* tokens, int token_len);
#endif