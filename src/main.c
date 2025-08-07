#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "asm.h"
#include "ast.h"
#include "lex.h"
#include "parse.h"
#include "symtab.h"
#include "util.h"

void my_lang(char *file_name)
{
    size_t input_length = 0;
    char *input = read_file(file_name, &input_length);
    SymTab *table = symtab_new();
    symtab_init(table);

    Vector *tokens = tokenize(input, input_length);

    for (size_t i = 0; i < tokens->length; i++)
        print_token((Token *)vector_get(tokens, i));

    Vector *prog = parse(tokens, table);

    free_tokens(tokens);

    gen_asm(prog, table);

    for (size_t i = 0; i < prog->length; i++)
        ast_free((AstNode *)vector_get(prog, i));

    symtab_free(table);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide an input file.");
        return 0;
    }
    my_lang(argv[1]);

    return 0;
}
