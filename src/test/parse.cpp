#include <gtest/gtest.h>
extern "C"
{
#include "../ast.h"
#include "../hashmap.h"
#include "../lex.h"
#include "../parse.h"
#include "../util.h"
}

TEST(Parse, Variable)
{
    Vector *tokens;
    TokenStream *stream;
    HashMap *symtab;
    AstNode *var;

    tokens = vector_new();
    symtab = hashmap_new();
    symtab_init(symtab);

    vector_push(tokens, make_token(TOK_TYPE, string((char *)"int"), 0, 0));
    vector_push(tokens, make_token(TOK_IDENT, string((char *)"var"), 0, 0));
    vector_push(tokens, make_token(TOK_ASSIGN, string((char *)"="), 0, 0));
    vector_push(tokens, make_token(TOK_NUM, string((char *)"10"), 0, 0));

    stream = make_token_stream(tokens);

    var = parse_var_def(stream, symtab);

    EXPECT_EQ(AST_VAR_DEF, var->type);

    symtab_free(symtab);
    free_tokens(tokens);
    free(stream);
    ast_free(var);
}