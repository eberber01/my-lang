#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "util.h"

typedef enum TokenType {
  KEYWORD,
  IDENT,
  NUM,
  ADD,
  SUB,
  MULT,
  DIV,
  LBRACKET,
  RBRACKET,
  LPAREN,
  RPAREN,
  LCBRACKET,
  RCBRACKET,
  SEMICOLON,
} TokenType;

typedef struct Token {
  int type;
  char* value;
} Token;

void print_token(Token* t);


char* int_to_str(int num, int size);

Token* make_token(int type, char* value);

int is_operator(char c);

char next(FILE *f);

Token* tokenize_digit(char c, FILE *f);

int is_ident_start(char c);

int is_ident_char(char c);

Token* tokenize_ident(char c, FILE* f);


Vector* tokenize(FILE *f);

#endif