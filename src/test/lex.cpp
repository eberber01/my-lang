
#include <gtest/gtest.h>
extern "C"
{
#include "../lex.h"
#include "../symtab.h"
#include "../util.h"
}

#define PUSH(v, type, value) vector_push(v, make_token(type, value, 0, 0))

TEST(Lex, Basic)
{
    char* input = "void main(){ int x = 10; x = hello; }";

    Vector* v = vector_new();
    PUSH(v, TOK_TYPE, "void");
    PUSH(v, TOK_IDENT, "main");
    PUSH(v, TOK_LPAREN, "(");
    PUSH(v, TOK_RPAREN, ")");
    PUSH(v, TOK_LBRACE, "{");
    PUSH(v, TOK_TYPE, "int");
    PUSH(v, TOK_IDENT, "x");
    PUSH(v, TOK_ASSIGN, "=");
    PUSH(v, TOK_NUM, "10");
    PUSH(v, TOK_SEMICOLON, ";");
    PUSH(v, TOK_IDENT, "x");
    PUSH(v, TOK_ASSIGN, "=");
    PUSH(v, TOK_IDENT, "hello");
    PUSH(v, TOK_SEMICOLON, ";");
    PUSH(v, TOK_RBRACE, "}");

    Vector* tokens = tokenize(input, strlen(input));
    EXPECT_EQ(tokens->length, v->length);

    Token* tok;
    Token* exp;
    for(int i =0; i< tokens->length; i++)
    {
        tok = (Token*)vector_get(tokens, i);
        exp = (Token*)vector_get(v, i);

        EXPECT_EQ(tok->type, exp->type);
        EXPECT_STREQ(tok->value, exp->value);
    }

}