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
  LBRACE,
  RBRACE,
  SEMICOLON,
  ASSIGN, 
  TYPE, 
  COMMA,
  RETURN,
  IF,
  EQUAL
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