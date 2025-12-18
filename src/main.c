#include "./llvm/backend.h"
#include <mylang/asm.h>
#include <mylang/ast.h>
#include <mylang/hashmap.h>
#include <mylang/lex.h>
#include <mylang/parse.h>
#include <mylang/sema.h>
#include <mylang/util.h>
#include <stdio.h>

typedef struct CompilerOptions
{
    bool use_llvm;
    char *file_name;
} CompilerOptions;

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

void my_lang(CompilerOptions *opt)
{
    size_t input_length = 0;
    char *input = read_file(opt->file_name, &input_length);

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

    if (opt->use_llvm)
        llvm_code_gen(prog);
    else
        gen_asm(prog);

    my_lang_cleanup(input, type_env, tokens, prog, symbols);
}

CompilerOptions *parse_args(int argc, char **argv)
{
    CompilerOptions *opt = my_malloc(sizeof(CompilerOptions));
    opt->use_llvm = false;
    if (argc < 2)
    {
        printf("Please provide an input file.");
        return 0;
    }

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp("--backend=llvm", argv[i]))
            opt->use_llvm = true;
    }

    opt->file_name = argv[argc - 1];

    return opt;
}

int main(int argc, char **argv)
{
    CompilerOptions *opt = parse_args(argc, argv);
    my_lang(opt);

    return 0;
}
