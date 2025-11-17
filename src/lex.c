#include "lex.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

char *read_file(char *filename, size_t *length)
{
    size_t n;
    FILE *f = fopen(filename, "rb");
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1); // +1 for null-terminator
    n = fread(buffer, 1, size, f);

    if (n < (size_t)size)
        perror("Failed to read file");

    buffer[size] = '\0';
    fclose(f);

    if (length)
        *length = size;
    return buffer;
}

// Parse each character in input and
// return a Vector of tokens from contents
Vector *tokenize(const char *input, size_t length)
{
    char c;
    Vector *vector = vector_new();

    Lexer *lexer = (Lexer *)my_malloc(sizeof(Lexer));
    lexer->curr = 0;
    lexer->input = input;
    lexer->length = length;

    int pos = 1;
    int line = 1;
    char peek;
    while ((c = next(lexer)))
    {
        Token *t = my_malloc(sizeof(Token));
        switch (c)
        {
        case '+':
            t->type = TOK_ADD;
            t->value = string("+");
            break;
        case '-':
            t->type = TOK_SUB;
            t->value = string("-");
            break;
        case '*':
            t->type = TOK_MULT;
            t->value = string("*");
            break;
        case '/':
            // Handle Comments
            if ((peek = next(lexer)) == '/')
            {
                while ((peek = next(lexer)) && peek != '\n')
                    ;

                c = peek;
                line += 1;
                pos = 1;
            }
            else
            {
                back(lexer);
                t->type = TOK_DIV;
                t->value = string("/");
            }
            break;
        case ';':
            t->type = TOK_SEMICOLON;
            t->value = string(";");
            break;
        case '(':
            t->type = TOK_LPAREN;
            t->value = string("(");
            break;
        case ')':
            t->type = TOK_RPAREN;
            t->value = string(")");
            break;
        case '{':
            t->type = TOK_LBRACE;
            t->value = string("{");
            break;
        case '}':
            t->type = TOK_RBRACE;
            t->value = string("}");
            break;
        case '=':
            if ((peek = next(lexer)) == '=')
            {
                t->type = TOK_EQUAL;
                t->value = string("==");
            }
            else
            {
                back(lexer);
                t->type = TOK_ASSIGN;
                t->value = string("=");
            }
            break;
        case ',':
            t->type = TOK_COMMA;
            t->value = string(",");
            break;
        case '\n':
            line += 1;
            pos = 1;
            break;
        case '\t':
        case ' ':
            break;
        default:

            if (isdigit(c))
            {
                tokenize_digit(c, t, lexer);
                break;
            }

            if (is_ident_start(c))
            {
                tokenize_ident(c, t, lexer);
                break;
            }
            printf("%d\n", c);
            perror("Unexpected char.");
            exit(1);
        }
        t->pos = pos;
        t->line = line;
        pos += 1;
        if (c != ' ' && c != '\n' && c != '\t')
        {
            vector_push(vector, (void *)t);
        }

        if (t->value == NULL)
        {
            free(t);
        }
    }
    free(lexer);
    return vector;
}

// Parse digit token
void tokenize_digit(char c, Token *token, Lexer *lexer)
{
    int size = 1;
    int digit = 0;
    int multiplier = 10;
    while (c && isdigit(c))
    {
        digit *= multiplier;
        digit += c - '0';
        c = next(lexer);
        size += 1;
    }
    // Put back last char if end of stream
    if (c != '\0')
    {
        back(lexer);
    }
    char *value = int_to_str(digit, size);
    token->value = string(value);
    token->type = TOK_NUM;
    free(value);
}

// Parse Identifier token
void tokenize_ident(char c, Token *token, Lexer *lexer)
{
    String *ident = string_new();
    while (c && is_ident_char(c))
    {
        string_append(ident, c);
        c = next(lexer);
    }

    // Put back last char if end of stream
    if (c != '\0')
    {
        back(lexer);
    }

    token->value = ident;
    token->type = TOK_IDENT;

    // Handle Keywords
    if (string_eq(ident, "int"))
        token->type = TOK_TYPE;
    else if (string_eq(ident, "void"))
        token->type = TOK_TYPE;
    else if (string_eq(ident, "return"))
        token->type = TOK_RETURN;
    else if (string_eq(ident, "if"))
        token->type = TOK_IF;
    else if (string_eq(ident, "enum"))
        token->type = TOK_ENUM;
    else if (string_eq(ident, "while"))
        token->type = TOK_WHILE;
}

// Put back character in stream
void back(Lexer *lexer)
{
    lexer->curr--;
}

// Get next character in stream
char next(Lexer *lexer)
{
    if (lexer->curr >= lexer->length)
    {
        return '\0';
    }
    return lexer->input[lexer->curr++];
}

Token *make_token(int type, String *value, int pos, int line)
{
    Token *t;
    t = my_malloc(sizeof(Token));

    t->type = type;
    t->value = value;
    t->pos = pos;
    t->line = line;
    return t;
}

void print_token(Token *t)
{
    char *str = as_str(t->value);
    printf("<TOKEN TYPE=%d VALUE=%s POS=%d LINE=%d>\n", t->type, str, t->pos, t->line);
    free(str);
}

int is_ident_char(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

int is_ident_start(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

void free_tokens(Vector *tokens)
{
    Token *token;
    for (size_t i = 0; i < tokens->length; i++)
    {
        token = (Token *)vector_get(tokens, i);
        string_free(token->value);
        free(token);
    }
    vector_free(tokens);
}
