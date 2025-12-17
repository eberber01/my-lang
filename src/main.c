#include "./llvm/backend.h"
#include <mylang/asm.h>
#include <mylang/ast.h>
#include <mylang/hashmap.h>
#include <mylang/lex.h>
#include <mylang/parse.h>
#include <mylang/sema.h>
#include <mylang/util.h>
#include <stdio.h>

void my_lang_cleanup(char *input, HashMap *type_env, Vector *tokens, Vector *prog, Vector *symbols)
{
    type_env_free(type_env);
    free_tokens(tokens);

    for (size_t i = 0; i < prog->length; i++)
        ast_free((AstNode *)vector_get(prog, i));
    vector_free(prog);

    for (size_t i = 0; i < symbols->length; i++)
    {
        SymTabEntry *symbol = (SymTabEntry *)vector_get(symbols, i);
        if (symbol->symbol == SYM_FUNCTION)
            free(symbol->frame);
        free(symbol->key);
        free(symbol);
    }
    vector_free(symbols);
    free(input);
}

void my_lang(char *file_name)
{
    size_t input_length = 0;
    char *input = read_file(file_name, &input_length);

    HashMap *type_env = hashmap_new();
    type_env_init(type_env);

    Vector *tokens = tokenize(input, input_length);

    Vector *prog = parse(tokens);

    Vector *symbols = sema_check(prog, type_env);
#ifdef DEBUG
    for (size_t i = 0; i < tokens->length; i++)
        print_token((Token *)vector_get(tokens, i));

    print_ast(prog);
#endif
    // gen_asm(prog);

    llvm_code_gen(prog);
    my_lang_cleanup(input, type_env, tokens, prog, symbols);
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
