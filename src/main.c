#include <mylang/arena.h>
#include <mylang/asm.h>
#include <mylang/ast.h>
#include <mylang/hashmap.h>
#include <mylang/lex.h>
#include <mylang/parse.h>
#include <mylang/sema.h>
#include <mylang/util.h>
#include <mylang/version.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct CompilerOptions
{
    char *file_name;
    bool dump_ast;
    bool dump_tokens;
} CompilerOptions;

struct CompilerOptions opt;

void my_lang(CompilerOptions opt)
{
    size_t input_length = 0;
    char *input = read_file(opt.file_name, &input_length);

    HashMap *type_env = hashmap_new();
    type_env_init(type_env);

    Vector *tokens = tokenize(input, input_length);

    Vector *prog = parse(tokens);

    sema_check(prog, type_env);

    if (opt.dump_tokens)
    {
        dump_tokens(tokens);
        exit(0);
    }

    if (opt.dump_ast)
    {
        dump_ast(prog);
        exit(0);
    }

    gen_asm(prog);

    type_env_free(type_env);
}

void print_version()
{
    printf("my-lang %s\n", MYLANG_VERSION);
    printf("Target: riscv32\n");
}

void print_help()
{

    char *message = "Usage: my-lang [options] file.c\n\n"
                    "Options:\n"
                    "\t-v, --version    Print version and exit\n"
                    "\t-h, --help       Show this help\n"
                    "\t-dump-tokens     Print Tokens\n"
                    "\t-dump-ast        Print AST\n";
    printf("%s", message);
}

void parse_args(int argc, char **argv)
{

    if (argc < 2)
    {
        printf("Please provide an input file.\n");
        exit(0);
    }

    if (!strcmp("-h", argv[1]) || !strcmp("--help", argv[1]))
    {
        print_help();
        exit(0);
    }

    if (!strcmp("-v", argv[1]) || !strcmp("--version", argv[1]))
    {
        print_version();
        exit(0);
    }

    for (int i = 0; i < argc - 1; i++)
    {
        if (!strcmp("-dump-tokens", argv[i]))
            opt.dump_tokens = true;
        if (!strcmp("-dump-ast", argv[i]))
            opt.dump_ast = true;
    }

    opt.file_name = argv[argc - 1];
}

int main(int argc, char **argv)
{
    parse_args(argc, argv);

    my_lang(opt);

    arena_free(context_arena);

    return 0;
}
