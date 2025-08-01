#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "asm.h"
#include "ast.h"
#include "lex.h"
#include "parse.h"
#include "symtab.h"
#include "util.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide an input file.");
        return 0;
    }
    char *file_name = argv[1];
    printf("%s\n", file_name);

    FILE *f = fopen(file_name, "r");
    if (f == NULL)
    {
        printf("Failed to open file.");
        return 1;
    }

    SymTab *table = symtab_new();
    symtab_init(table);

    Vector *tokens = tokenize(f, table);

    for (int i = 0; i < tokens->length; i++)
    {
        print_token((Token *)vector_get(tokens, i));
    }

    AstNode *tree = parse(tokens, table);
    print_ast_tree(tree);
    //gen_asm(tree, table);

    fclose(f);
    // Bruh what is this
    //  for(int i =0; i < tokens->length; i++){
    //    Token* token = (Token*)vector_get(tokens, i);
    //    if( token->type == TOK_NUM  || token->type == TOK_IDENT ){
    //      free(token->value);
    //    }
    //  }
    // vector_free(tokens);
    // ast_free(tree);
    // symtab_free(table);
    return 0;
}