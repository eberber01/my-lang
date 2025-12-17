
#include <gtest/gtest.h>
extern "C"
{
#include <mylang/lex.h>
#include <mylang/util.h>
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
    ::testing::Values(
        LexCase{"void main(){ int x=10; }",
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
        LexCase{"//comment\nx=1;", {{TOK_IDENT, "x"}, {TOK_ASSIGN, "="}, {TOK_NUM, "1"}, {TOK_SEMICOLON, ";"}}},
        // Basic function
        LexCase{"void main(){ int x=10; }",
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
                 {TOK_RBRACE, "}"}}},

        // Single-line comment
        LexCase{"//comment\nx=1;", {{TOK_IDENT, "x"}, {TOK_ASSIGN, "="}, {TOK_NUM, "1"}, {TOK_SEMICOLON, ";"}}},

        // Operators
        LexCase{"+ - * / == != ;",
                {{TOK_ADD, "+"},
                 {TOK_SUB, "-"},
                 {TOK_MULT, "*"},
                 {TOK_DIV, "/"},
                 {TOK_EQUAL, "=="},
                 {TOK_NOT_EQUAL, "!="},
                 {TOK_SEMICOLON, ";"}}},

        // Braces and parentheses
        LexCase{"(){}", {{TOK_LPAREN, "("}, {TOK_RPAREN, ")"}, {TOK_LBRACE, "{"}, {TOK_RBRACE, "}"}}},

        // Keywords
        LexCase{"return if else while for int void",
                {{TOK_RETURN, "return"},
                 {TOK_IF, "if"},
                 {TOK_ELSE, "else"},
                 {TOK_WHILE, "while"},
                 {TOK_FOR, "for"},
                 {TOK_TYPE, "int"},
                 {TOK_TYPE, "void"}}},

        // Identifiers and digits
        LexCase{"foo bar123 42", {{TOK_IDENT, "foo"}, {TOK_IDENT, "bar123"}, {TOK_NUM, "42"}}},

        // Mixed whitespace
        LexCase{"\t\n x = 5 ; \n", {{TOK_IDENT, "x"}, {TOK_ASSIGN, "="}, {TOK_NUM, "5"}, {TOK_SEMICOLON, ";"}}},

        // Complex expression
        LexCase{"x = a + b * (c - 1) / d;",
                {{TOK_IDENT, "x"},
                 {TOK_ASSIGN, "="},
                 {TOK_IDENT, "a"},
                 {TOK_ADD, "+"},
                 {TOK_IDENT, "b"},
                 {TOK_MULT, "*"},
                 {TOK_LPAREN, "("},
                 {TOK_IDENT, "c"},
                 {TOK_SUB, "-"},
                 {TOK_NUM, "1"},
                 {TOK_RPAREN, ")"},
                 {TOK_DIV, "/"},
                 {TOK_IDENT, "d"},
                 {TOK_SEMICOLON, ";"}}}

        ));
