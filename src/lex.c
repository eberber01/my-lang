#include "lex.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtab.h"
#include "util.h"

// Parse each character in input and
// return a Vector of tokens from contents
// Uses Symbol table to get standard types
Vector *tokenize(FILE *f)
{
    char c;
    Vector *vector = vector_new();

    int pos = 1;
    int line = 1;
    char peek;
    while ((c = next(f)))
    {
        Token *t = my_malloc(sizeof(Token));
        switch (c)
        {
        case '+':
            t->type = TOK_ADD;
            t->value = "+";
            break;
        case '-':
            t->type = TOK_SUB;
            t->value = "-";
            break;
        case '*':
            t->type = TOK_MULT;
            t->value = "*";
            break;
        case '/':
            // Handle Comments
            if ((peek = next(f)) == '/')
            {
                while (peek && peek != '\n')
                {
                    peek = next(f);
                }
                c = peek;
                line += 1;
                pos = 1;
            }
            else
            {
                back(f);
                t->type = TOK_DIV;
                t->value = "/";
            }
            break;
        case ';':
            t->type = TOK_SEMICOLON;
            t->value = ";";
            break;
        case '(':
            t->type = TOK_LPAREN;
            t->value = "(";
            break;
        case ')':
            t->type = TOK_RPAREN;
            t->value = ")";
            break;
        case '{':
            t->type = TOK_LBRACE;
            t->value = "{";
            break;
        case '}':
            t->type = TOK_RBRACE;
            t->value = "}";
            break;
        case '=':
            char peek;
            if ((peek = next(f)) == '=')
            {
                t->type = TOK_EQUAL;
                t->value = "==";
            }
            else
            {
                back(f);
                t->type = TOK_ASSIGN;
                t->value = "=";
            }
            break;
        case ',':
            t->type = TOK_COMMA;
            t->value = ",";
            break;
        case ' ':
            break;
        case '\n':
            line += 1;
            pos = 1;
            break;
        default:

            if (isdigit(c))
            {
                tokenize_digit(c, t, f);
                break;
            }

            if (is_ident_start(c))
            {
                tokenize_ident(c, t, f);
                break;
            }

            perror("Unexpected char.");
            exit(1);
        }
        t->pos = pos;
        t->line = line;
        pos += 1;
        if (c != ' ' && c != '\n')
        {
            vector_push(vector, (void *)t);
        }

        if (t->value == NULL)
        {
            free(t);
        }
    }
    return vector;
}

// Parse digit token
void tokenize_digit(char c, Token *token, FILE *f)
{
    int size = 1;
    int digit = 0;
    int multiplier = 10;
    while (c && isdigit(c))
    {
        digit *= multiplier;
        digit += c - '0';
        c = next(f);
        size += 1;
    }
    // Put back last char if end of stream
    if (c != '\0')
    {
        back(f);
    }
    token->value = int_to_str(digit, size);
    token->type = TOK_NUM;
}

// Parse Identifier token
void tokenize_ident(char c, Token *token, FILE *f)
{
    String *ident = string_new();
    while (c && is_ident_char(c))
    {
        string_append(ident, c);
        c = next(f);
    }

    // Put back last char if end of stream
    if (c != '\0')
    {
        back(f);
    }

    char *value = as_str(ident);
    string_free(ident);

    token->value = value;
    token->type = TOK_IDENT;

    // Handle Keywords
    if (!strcmp(value, "int"))
        token->type = TOK_TYPE;
    else if (!strcmp(value, "void"))
        token->type = TOK_TYPE;
    else if (!strcmp(value, "return"))
        token->type = TOK_RETURN;
    else if (!strcmp(value, "if"))
        token->type = TOK_IF;
    else if (!strcmp(value, "enum"))
        token->type = TOK_ENUM;
}

// Put back character in stream
void back(FILE *f)
{
    fseek(f, -sizeof(char), SEEK_CUR);
}

// Get next character in stream
char next(FILE *f)
{
    char c;
    int n = fread(&c, sizeof(char), 1, f);
    if (n < 1)
    {
        return '\0';
    }
    return c;
}

Token *make_token(int type, char *value, int pos, int line)
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
    printf("<TOKEN TYPE=%d VALUE=%s POS=%d LINE=%d>\n", t->type, t->value, t->pos, t->line);
}

int is_ident_char(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
}

int is_ident_start(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}