#include "asm.h"
#include "ast.h"
#include "lex.h"
#include "parse.h"
#include "sema.h"
#include "symtab.h"
#include "util.h"
#include <stdio.h>

// #define DEBUG

void my_lang(char *file_name)
{
    size_t input_length = 0;
    char *input = read_file(file_name, &input_length);
    SymTab *table = symtab_new();
    symtab_init(table);

    Vector *tokens = tokenize(input, input_length);

#ifdef DEBUG
    for (size_t i = 0; i < tokens->length; i++)
        print_token((Token *)vector_get(tokens, i));
#endif

    Vector *prog = parse(tokens, table);

    free_tokens(tokens);
    print_ast(prog);
    exit(0);
    // sema_check(prog, table);
    // gen_asm(prog);

    for (size_t i = 0; i < prog->length; i++)
        ast_free((AstNode *)vector_get(prog, i));
    vector_free(prog);
    // symtab_free(table);
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
