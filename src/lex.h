#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "symtab.h"
#include "util.h"

typedef enum TokenType {
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
  TOK_EQUAL
} TokenType;

typedef struct Token {
  TokenType type;
  char* value;
  int pos;
  int line;
} Token;

void print_token(Token* t);

Token* make_token(int type, char* value, int pos, int line);

int is_operator(char c);

char next(FILE *f);

void tokenize_digit(char c, Token* token, FILE *f);

int is_ident_start(char c);

int is_ident_char(char c);

void tokenize_ident(char c, Token* token, SymTab* table, FILE *f);

Vector* tokenize(FILE *f, SymTab* table);

void back(FILE* );
#endif