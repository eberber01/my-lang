#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ast.h"
#include "lex.h"
#include "parse.h"
#include "asm.h"

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

  int token_len;
  struct Token * tokens;
  tokens = tokenize(f, &token_len);


  for(int i =0; i < token_len; i++){
    print_token(tokens[i]);
  }

  struct AstNode* tree = parse(tokens, token_len);
  gen_asm(tree);

  fclose(f);

  for(int i =0; i < token_len; i++){
    if(tokens[i].type == IDENT || tokens[i].type == NUM){
      free(tokens[i].value);
    }
  }
  free(tokens);

  return 0;
}