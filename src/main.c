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
    size_t input_length = 0;
    char *input = read_file(file_name, &input_length);
    SymTab *table = symtab_new();
    symtab_init(table);

    Vector *tokens = tokenize(input, input_length);

    for (int i = 0; i < tokens->length; i++)
    {
        print_token((Token *)vector_get(tokens, i));
    }

    Vector *prog = parse(tokens, table);

    gen_asm(prog, table);

    for (int i = 0; i < tokens->length; i++)
    {
        Token *token = (Token *)vector_get(tokens, i);
        if (token->type == TOK_NUM || token->type == TOK_IDENT)
        {
            free(token->value);
        }
        free(token);
    }
    free(tokens);

    // vector_free(tokens);
    // for(int i = 0; i < prog->length; i++)
    // ast_free((AstNode*)vector_get(prog, i));
    // symtab_free(table);
    return 0;
}