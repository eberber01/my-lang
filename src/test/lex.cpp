
#include <gtest/gtest.h>
extern "C"
{
#include "../lex.h"
#include "../symtab.h"
#include "../util.h"
}

struct LexCase
{
    const char *input;
    std::vector<std::pair<int, const char *>> expected;
};

class LexParamTest : public ::testing::TestWithParam<LexCase>
{
};

TEST_P(LexParamTest, MatchesExpectedTokens)
{
    auto [in, expected] = GetParam();
    Vector *tokens = tokenize(in, strlen(in));
    ASSERT_EQ(tokens->length, expected.size());
    for (size_t i = 0; i < expected.size(); ++i)
    {
        Token *t = (Token *)vector_get(tokens, i);
        char *str = as_str(t->value);
        EXPECT_EQ(t->type, expected[i].first);
        EXPECT_STREQ(str, expected[i].second);
        string_free(t->value);
        free(t);
        free(str);
    }
    free(tokens);
}

INSTANTIATE_TEST_SUITE_P(
    LexerTests, LexParamTest,
    ::testing::Values(LexCase{"void main(){ int x=10; }",
                              {{TOK_TYPE, "void"},
                               {TOK_IDENT, "main"},
                               {TOK_LPAREN, "("},
                               {TOK_RPAREN, ")"},
                               {TOK_LBRACE, "{"},
                               {TOK_TYPE, "int"},
                               {TOK_IDENT, "x"},
                               {TOK_ASSIGN, "="},
                               {TOK_NUM, "10"},
                               {TOK_SEMICOLON, ";"},
                               {TOK_RBRACE, "}"}}}

                      ,
                      LexCase{"//comment\nx=1;",
                              {{TOK_IDENT, "x"}, {TOK_ASSIGN, "="}, {TOK_NUM, "1"}, {TOK_SEMICOLON, ";"}}}));
