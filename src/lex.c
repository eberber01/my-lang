#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "lex.h"
#include "util.h"

Vector* tokenize(FILE *f) {
  char c;
  Vector* vector = vector_new();

  int pos = 0;
  int line = 0;
  while ((c = next(f))) {
    Token* t = my_malloc(sizeof(Token));
    switch (c) {
    case '+':
      t->type = ADD;
      t->value = "+";
      break;
    case '-':
      t->type = SUB;
      t->value = "-";
      break;
    case '*':
      t->type = MULT;
      t->value = "*";
      break; case '/':
      t->type = DIV;
      t->value = "/";
      break;
    case ';':
      t->type = SEMICOLON;
      t->value = ";";
      break;
    case '(':
      t->type = LPAREN;
      t->value = "(";
      break;
    case ')':
      t->type = RPAREN;
      t->value = ")";
      break;
    case '{':
      t->type = LCBRACKET;
      t->value = "{";
      break;
    case '}':
      t->type = RCBRACKET;
      t->value = "}";
      break;
    case '=':
      t->type = ASSIGN;
      t->value = "=";
      break;
    case ',':
      t->type = ASSIGN;
      t->value = ",";
      break;
    case ' ':
      break;
    case '\n':
      line += 1;
      pos = 0;
      break;
    default:  

      if(isdigit(c)){

        //free this 
        Token* digit_token = tokenize_digit(c, f);
        t->type = digit_token->type;
        t->value = digit_token->value;
        free(digit_token);
        break;
      }

      if(is_ident_start(c)){
        //free this 
        Token* ident_token =  tokenize_ident(c, f);
        t->type = ident_token->type;
        if(!strcmp(ident_token->value , "int")){

          t->type = TYPE;
        }
        t->value = ident_token->value;
        free(ident_token);
        break;
      }


      perror("Unexpected char.");
      exit(1);
    }
    t->pos = pos;
    t->line = line;
    pos += 1;
    if(c != ' ' && c != '\n'){
      vector_push(vector,  t);
    }

  }
  return vector;
}

Token* tokenize_digit(char c, FILE *f){
  int size = 1;
  int digit = 0;
  int multiplier = 10;
  while(c && isdigit(c)){
    digit *= multiplier;
    digit += c - '0';
    c = next(f);
    size += 1;
  }
  if(c != '\0'){
    // if not end of stream putback last char 
    fseek(f, -sizeof(char), SEEK_CUR);
  }
  return make_token(NUM, int_to_str(digit, size), 0, 0); 
}

int is_ident_start(char c){
  return (c >= 'A'   && c <= 'Z') || (c >= 'a'   && c <= 'z') || c == '_';
}

int is_ident_char(char c){
  return (c >= 'A'   && c <= 'Z') || (c >= 'a'   && c <= 'z') || ( c >= '0' && c <= '9') || c == '-' || c == '_';
}

Token* tokenize_ident(char c, FILE* f){
  String* ident = string_new();
  while(c && is_ident_char(c)){
    string_append(ident, c);
    c = next(f);
  }
  if(c != '\0'){
    // if not end of stream putback last char 
    fseek(f, -sizeof(char), SEEK_CUR);
  }

  char* value = as_str(ident);
  string_free(ident);
  return make_token(IDENT, value, 0, 0);
}

char next(FILE *f) {
  char c;
  int n = fread(&c, sizeof(char), 1, f);
  if (n < 1) {
    return '\0';
  }
  return c;
}

Token* make_token(int type, char* value, int pos, int line){
  Token* t;
  t = my_malloc(sizeof(Token));

  t->type = type;
  t->value = value;
  t->pos = pos;
  t->line = line;
  return t;
}

void print_token(Token* t) {
  printf("<TOKEN TYPE=%d VALUE=%s POS=%d LINE=%d>\n", t->type, t->value, t->pos, t->line);
}

char* int_to_str(int num, int size){
  char *str = my_malloc((sizeof(char) * size ) + 1);
  snprintf(str, size, "%d", num); 
  return str;
}

int is_operator(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }