#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hashmap.h"
#include "util.h"

typedef struct Lexer
{
    const char *input;
    int curr;
    int length;
} Lexer;

typedef enum TokenType
{
    TOK_IDENT,
    TOK_NUM,
    TOK_ADD,
    TOK_SUB,
    TOK_MULT,
    TOK_DIV,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMICOLON,
    TOK_ASSIGN,
    TOK_TYPE,
    TOK_COMMA,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_EQUAL,
    TOK_NOT_EQUAL,
    TOK_NOT,
    TOK_ENUM,
    TOK_WHILE,
    TOK_FOR,
    TOK_AND,
    TOK_OR,
    TOK_XOR,
    TOK_LOG_OR,
    TOK_LOG_AND,
    TOK_LSHIFT,
    TOK_RSHIFT,
    TOK_GT,
    TOK_LT,
    TOK_GT_EQ,
    TOK_LT_EQ,
} TokenType;

typedef struct Token
{
    TokenType type;
    String *value;
    int pos;
    int line;
} Token;

void print_token(Token *t);

Token *make_token(int type, String *value, int pos, int line);

int is_operator(char c);

char next(Lexer *Lexer);

void tokenize_digit(char c, Token *token, Lexer *lexer);

int is_ident_start(char c);

int is_ident_char(char c);

void tokenize_ident(char c, Token *token, Lexer *lexer);

char *read_file(char *file_name, size_t *length);

Vector *tokenize(const char *input, size_t input_length);

void back(Lexer *lexer);

void free_tokens(Vector *tokens);

#endif
