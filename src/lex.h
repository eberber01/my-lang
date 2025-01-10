#ifndef LEX_H
#define LEX_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "symtab.h"
#include "util.h"

typedef enum TokenType {
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
  ASSIGN, 
  TYPE, 
  COMMA,
  RETURN,
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

Token* tokenize_digit(char c, FILE *f);

int is_ident_start(char c);

int is_ident_char(char c);

Token* tokenize_ident(char c, FILE* f);

Vector* tokenize(FILE *f, SymTab* table);

void back(FILE* );
#endif