#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum TokenType {
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
};

typedef struct Token {
  int type;
  char* value;
} Token;

void print_token(struct Token t);


char* int_to_str(int num, int size);

struct Token* make_token(int type, char* value);

int is_operator(char c);

char next(FILE *f);

struct Token* tokenize_digit(char c, FILE *f);

int is_ident_start(char c);

int is_ident_char(char c);

struct Token* tokenize_ident(char c, FILE* f);


struct Token* tokenize(FILE *f, int *token_len);

#endif