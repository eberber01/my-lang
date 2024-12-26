#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "lex.h"

struct Token* tokenize(FILE *f, int *token_len) {
  char c;
  struct Token t;

  int token_size = 5;
  *token_len = 0;
  struct Token* tokens = malloc(sizeof(Token) * token_size);
  if(tokens == NULL){
    perror("Failed to allocate memory.");
    exit(1);
  }

  int pos = 0;
  int line = 0;
  while ((c = next(f))) {
    switch (c) {
    case '+':
      t.type = ADD;
      t.value = "+";
      break;
    case '-':
      t.type = SUB;
      t.value = "-";
      break;
    case '*':
      t.type = MULT;
      t.value = "*";
      break;
    case '/':
      t.type = DIV;
      t.value = "/";
      break;
    case ';':
      t.type = SEMICOLON;
      t.value = ";";
      break;
    case '(':
      t.type = LPAREN;
      t.value = "(";
      break;
    case ')':
      t.type = RPAREN;
      t.value = ")";
      break;
    case '{':
      t.type = LCBRACKET;
      t.value = "{";
      break;
    case '}':
      t.type = RCBRACKET;
      t.value = "}";
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
        struct Token* digit_token = tokenize_digit(c, f);
        t.type = digit_token->type;
        t.value = digit_token->value;
        free(digit_token);
        break;
      }

      if(is_ident_start(c)){
        //free this 
        struct Token* ident_token =  tokenize_ident(c, f);
        t.type = ident_token->type;
        t.value = ident_token->value;
        free(ident_token);
        break;
      }


      perror("Unexpected char.");
      exit(1);
    }

    pos += 1;
    if(c != ' ' && c != '\n'){
      print_token(t);

      if(*token_len >= token_size){
        token_size *= 2; //New size
        struct Token* new_tokens = realloc(tokens, sizeof(Token) * token_size);
        if(new_tokens == NULL){
          perror("Failed to allocate memory.");
          exit(1);
        }
        tokens = new_tokens;
        new_tokens = NULL;
      }

      tokens[*token_len].type = t.type;
      tokens[*token_len].value = t.value;
      *token_len += 1;
    }

  }
  return tokens;
}

struct Token* tokenize_digit(char c, FILE *f){
  int size = 1;
  int digit = 0;
  int multiplier = 10;
  while(c && isdigit(c)){
    digit *= multiplier;
    digit += c - '0';
    c = next(f);
    size += 1;
  }
  //put back last char
  fseek(f, -sizeof(char), SEEK_CUR);
  return make_token(NUM, int_to_str(digit, size)); 
}

int is_ident_start(char c){
  return (c >= 'A'   && c <= 'Z') || (c >= 'a'   && c <= 'z');
}

int is_ident_char(char c){
  return (c >= 'A'   && c <= 'Z') || (c >= 'a'   && c <= 'z');
}

struct Token* tokenize_ident(char c, FILE* f){
  char* ident = malloc(sizeof(char) * 10);
  if(ident == NULL){
    perror("Failed to allocate memory.");
    exit(1);
  }

  int i =0;
  while(c && is_ident_char(c)){
    if(i > 9){
      char* new_ident = realloc(ident, sizeof(char) * (2 * i));
      if(new_ident == NULL){
        perror("Failed to allocate memory.");
        exit(1);
      }
      ident = new_ident;
      new_ident = NULL;

    }
    ident[i] = c;
    c = next(f);
    i++;
  }
  ident[i] = '\0'; // Null terminate

  //putback last char
  fseek(f, -sizeof(char), SEEK_CUR);
  return make_token(IDENT, ident);
}

char next(FILE *f) {
  char c;
  int n = fread(&c, sizeof(char), 1, f);
  if (n < 1) {
    return '\0';
  }
  return c;
}

struct Token* make_token(int type, char* value){
  struct Token* t;
  t = malloc(sizeof(Token));

  if(t == NULL){
    perror("Failed to allocate memory.");
    exit(1);
  }
  t->type = type;
  t->value = value;
  return t;
}

void print_token(struct Token t) {
  printf("<TOKEN TYPE=%d VALUE=%s>\n", t.type, t.value);
}

char* int_to_str(int num, int size){
  char *str = malloc((sizeof(char) * size ) + 1);
  snprintf(str, size, "%d", num); 
  return str;
}

int is_operator(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }