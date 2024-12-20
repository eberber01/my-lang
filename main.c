#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

enum TokenType {
  NUM,
  ADD,
  SUB,
  MULT,
  DIV,
};

struct Token {
  int type;
  int value;
} Token;

void print_token(struct Token t) {
  printf("<TOKEN TYPE=%d VALUE=%c>", t.type, t.value);
}

int is_operator(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }

char next(FILE *f) {
  char c;
  int n = fread(&c, sizeof(char), 1, f);
  if (n < 1) {
    return '\0';
  }
  return c;
}

char parse_digit(char c, FILE *f){
  int digit = 0;
  int multiplier = 10;
  while(c && isdigit(c)){
    digit *= multiplier;
    digit += c - '0';
    c = next(f);
  }
  printf("Digit: %d\n", digit);
  return c;
}


void parse(FILE *f) {
  char c;
  struct Token t;
  struct Token** tokens;
  int token_len;
  int pos = 0;
  int line = 0;
  while ((c = next(f))) {

    if(isdigit(c)){
      c = parse_digit(c, f);
    }

    switch (c) {
    case '+':
      t.type = ADD;
      t.value = '+';
      break;
    case '-':
      t.type = SUB;
      t.value = '-';
      break;
    case '*':
      t.type = MULT;
      t.value = '*';
      break;
    case '/':
      t.type = DIV;
      t.value = '/';
      break;
    case ' ':
      break;
    case '\n':
      line += 1;
      pos = 0;
      break;
    default:
      perror("Unexpected char.");
      exit(1);
    }

    pos += 1;
    if(c != ' ' && c != '\n'){
      
      if(tokens){

      }


    }
  }
}



int main(int argc, char **argv) {

  if (argc < 2) {
    printf("Please provide an input file.");
    return 0;
  }
  char *file_name = argv[1];
  printf("%s\n", file_name);

  FILE *f = fopen(file_name, "r");
  if (f == NULL) {
    printf("Failed to open file.");
    return 1;
  }
  parse(f);
  fclose(f);
  return 0;
}