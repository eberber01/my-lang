#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"
#include "symtab.h"
#include "util.h"

// Parse each character in input and
// return a Vector of tokens from contents
// Uses Symbol table to get standard types
Vector* tokenize(FILE *f, SymTab* table) {
  char c;
  Vector* vector = vector_new();

  int pos = 1;
  int line = 1;
  char peek;
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
      break; 
      case '/':
        // Handle Comments
        if((peek = next(f)) == '/'){
          while(peek && peek != '\n'){
              peek = next(f);
          }
          c = peek;
          line += 1;
          pos = 1;
        }
        else{
          back(f);
          t->type = DIV;
          t->value = "/";
        }
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
      pos = 1;
      break;
    default:  

      if(isdigit(c)){
        Token* digit_token = tokenize_digit(c, f);
        t->type = digit_token->type;
        t->value = digit_token->value;
        free(digit_token);
        break;
      }

      if(is_ident_start(c)){
        Token* ident_token =  tokenize_ident(c, f);
        t->type = ident_token->type;
        SymTabEntry* entry;
        entry = symtab_get(table, ident_token->value);
          if(entry){
            t->type = TYPE;
          }
          if(!strcmp(ident_token->value, "return")){
            t->type = RETURN;
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

//Parse digit token
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
  //Put back last char if end of stream
  if(c != '\0'){
    back(f);
  }
  return make_token(NUM, int_to_str(digit, size), 0, 0); 
}

//Parse Identifier token
Token* tokenize_ident(char c, FILE* f){
  String* ident = string_new();
  while(c && is_ident_char(c)){
    string_append(ident, c);
    c = next(f);
  }

  //Put back last char if end of stream
  if(c != '\0'){
    back(f);
  }

  char* value = as_str(ident);
  string_free(ident);
  return make_token(IDENT, value, 0, 0);
}

//Put back character in stream
void back(FILE *f){
  fseek(f, -sizeof(char), SEEK_CUR);
}

//Get next character in stream
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

int is_ident_char(char c){
  return (c >= 'A'   && c <= 'Z') || (c >= 'a'   && c <= 'z') || ( c >= '0' && c <= '9') || c == '_';
}

int is_ident_start(char c){
  return (c >= 'A'   && c <= 'Z') || (c >= 'a'   && c <= 'z') || c == '_';
}