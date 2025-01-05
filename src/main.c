#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ast.h"
#include "util.h"
#include "lex.h"
#include "parse.h"
#include "asm.h"
#include "symtab.h"

void symtab_init(SymTab* table){
  symtab_add(table, make_symtab_entry("int", "", TYPE));
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

  Vector * tokens = tokenize(f);


  for(int i =0; i < tokens->length; i++){
    print_token((Token*)vector_get(tokens, i));
  }

  //AstNode* tree = parse(tokens, token_len);
  //gen_asm(tree);

  fclose(f);

  for(int i =0; i < tokens->length; i++){
    Token* token = (Token*)vector_get(tokens, i);
    if(token->type == IDENT || token->type == NUM){
      free(token->value);
    }
  }
  free(tokens);

  return 0;
}