#include <gtest/gtest.h>
extern "C"
{
#include <mylang/ast.h>
#include <mylang/hashmap.h>
#include <mylang/lex.h>
#include <mylang/parse.h>
#include <mylang/util.h>
}

TEST(Parse, Variable)
{
    Vector *tokens;
    TokenStream *stream;
    HashMap *symtab;
    AstNode *var;

    tokens = vector_new();

    vector_push(tokens, make_token(TOK_TYPE, string((char *)"int"), 0, 0));
    vector_push(tokens, make_token(TOK_IDENT, string((char *)"var"), 0, 0));
    vector_push(tokens, make_token(TOK_ASSIGN, string((char *)"="), 0, 0));
    vector_push(tokens, make_token(TOK_NUM, string((char *)"10"), 0, 0));

    stream = make_token_stream(tokens);

    var = parse_var_def(stream);

    EXPECT_EQ(AST_VAR_DEF, var->type);
    arena_free(context_arena);
}