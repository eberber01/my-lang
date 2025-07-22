// #include "ast.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ast.h"
#include "lex.h"
#include "symtab.h"
#include "util.h"

// Get current token in stream
// Returns NULL if end of stream
Token *current_token(TokenStream *stream)
{
    if (stream->current >= stream->tokens->length)
    {
        return NULL;
    }
    return (Token *)vector_get(stream->tokens, stream->current);
}

// Get next nth token in stream
// Returns NULL if outside of stream size
Token *peek(TokenStream *stream, int n)
{
    if (n + stream->current >= stream->tokens->length)
    {
        return NULL;
    }
    return (Token *)vector_get(stream->tokens, n + stream->current);
}

// Set next token in stream
void next_token(TokenStream *stream)
{
    if (stream->current >= stream->tokens->length)
    {
        return;
    }
    (stream->current)++;
}

TokenStream *make_token_stream(Vector *tokens)
{
    TokenStream *stream = my_malloc(sizeof(TokenStream));

    stream->tokens = tokens;
    stream->current = 0;
    return stream;
}

// Returns current token in stream
// if TokenType matches 'expect'
// errors otherwise
Token *expect(TokenStream *stream, TokenType expect)
{
    Token *current = current_token(stream);
    if (current == NULL)
    {
        return current;
    }
    if (current->type == expect)
    {
        next_token(stream);
        return current;
    }
    exit(1);
}

AstNode *parse_expression(TokenStream *stream, SymTab *table)
{
    AstNode *left = parse_term(stream, table);
    Token *current;
    while ((current = current_token(stream)) && (current->type == TOK_ADD || current->type == TOK_SUB))
    {
        char *value = current->value;
        next_token(stream);
        AstNode *right = parse_term(stream, table);
        left = make_ast_bin_exp(value, left, right);
    }
    return left;
}

AstNode *parse_term(TokenStream *stream, SymTab *table)
{
    AstNode *left = parse_factor(stream, table);
    Token *current;
    while ((current = current_token(stream)) && (current->type == TOK_DIV || current->type == TOK_MULT))
    {
        char *value = current->value;
        next_token(stream);
        AstNode *right = parse_factor(stream, table);
        left = make_ast_bin_exp(value, left, right);
    }
    return left;
}

int is_func_call_start(TokenStream *stream, SymTab *table)
{
    Token *func_name = current_token(stream);
    Token *lparen = peek(stream, 1);
    return func_name->type == TOK_IDENT && lparen->type == TOK_LPAREN;
}

AstNode *parse_func_call(TokenStream *stream, SymTab *table)
{
    Token *name = expect(stream, TOK_IDENT);
    // TODO:CHECK IF DEFINED IN SYMBOL TABLE

    expect(stream, TOK_LPAREN);

    Vector *args = vector_new();
    while (current_token(stream)->type != TOK_RPAREN)
    {
        // TODO Double check arg types
        AstNode *expr = parse_expression(stream, table);

        vector_push(args, expr);

        if (current_token(stream)->type == TOK_RPAREN)
        {
            break;
        }

        expect(stream, TOK_COMMA);
    }

    expect(stream, TOK_RPAREN);
    return make_ast_func_call(name->value, args);
}

AstNode *parse_factor(TokenStream *stream, SymTab *table)
{
    Token *current = current_token(stream);
    if (current->type == TOK_NUM)
    {
        next_token(stream);
        return make_int_const(current->value);
    }
    else if (current->type == TOK_IDENT)
    {
        if (is_func_call_start(stream, table))
        {
            return parse_func_call(stream, table);
        }

        if (symtab_get(table, current->value) == NULL)
        {
            perror("Cannot used undefined variable");
            exit(1);
        }
        next_token(stream);
        return make_ast_var(current->value);
    }
    else if (current->type == TOK_LPAREN)
    {
        next_token(stream);
        AstNode *ret = parse_expression(stream, table);

        expect(stream, TOK_RPAREN);

        return ret;
    }
    else
    {
        return NULL;
    }
}

int is_func_def_start(TokenStream *stream)
{
    Token *func_type = current_token(stream);
    Token *func_name = peek(stream, 1);
    Token *paren = peek(stream, 2);
    return func_type->type == TOK_TYPE && func_name->type == TOK_IDENT && paren->type == TOK_LPAREN;
}

int is_var_dec_start(TokenStream *stream)
{
    Token *var_type = current_token(stream);
    Token *var_name = peek(stream, 1);
    Token *assign = peek(stream, 2);
    return var_type->type == TOK_TYPE && var_name->type == TOK_IDENT && assign->type == TOK_ASSIGN;
}

AstNode *parse_boolean_expression(TokenStream *stream, SymTab *table)
{
    Token *current;
    AstNode *left = parse_expression(stream, table);
    while ((current = current_token(stream)) && (current->type == TOK_EQUAL))
    {
        char *value = current->value;
        next_token(stream);
        AstNode *right = parse_expression(stream, table);
        left = make_ast_bool_expr(value, left, right);
    }
    return left;
}

AstNode *parse_if_statement(TokenStream *stream, SymTab *table)
{
    expect(stream, TOK_IF);
    expect(stream, TOK_LPAREN);
    AstNode *expr = parse_boolean_expression(stream, table);

    expect(stream, TOK_RPAREN);
    expect(stream, TOK_LBRACE);

    Vector *if_body = parse_body(stream, table);

    expect(stream, TOK_RBRACE);
    return make_ast_if(expr, if_body);
}

AstNode *parse_statement(TokenStream *stream, SymTab *table)
{
    Token *current;
    Vector *body = vector_new();
    while ((current = current_token(stream)) && current_token(stream)->type != TOK_RBRACE)
    {
        if (is_func_def_start(stream))
        {
            // Variable declaration/assignment
            AstNode *func = parse_func_def(stream, table);

            vector_push(body, func);
        }
        else if (is_var_dec_start(stream))
        {
            // Variable declaration/assignment
            AstNode *var = parse_var_def(stream, table);
            vector_push(body, var);
        }
        else if (current->type == TOK_RETURN)
        {
            expect(stream, TOK_RETURN);
            AstNode *expr = parse_expression(stream, table);

            AstNode *ret_node = make_ast_ret(expr);
            vector_push(body, ret_node);
            expect(stream, TOK_SEMICOLON);
        }
        else if (current->type == TOK_IF)
        {
            AstNode *if_state = parse_if_statement(stream, table);
            vector_push(body, if_state);
        }
        else
        {
            AstNode *expr = parse_expression(stream, table);
            vector_push(body, expr);
            expect(stream, TOK_SEMICOLON);
        }
    }
    return make_ast_stmt(body);
}

Vector *parse_body(TokenStream *stream, SymTab *table)
{
    Vector *body = vector_new();
    while (current_token(stream) && current_token(stream)->type != TOK_RBRACE)
    {
        AstNode *stmt = parse_statement(stream, table);
        vector_push(body, stmt);
    }
    return body;
}

Vector *parse_func_params(TokenStream *stream, SymTab *table)
{
    Vector *params = vector_new();
    Token *current;
    while ((current = current_token(stream)) && current_token(stream)->type == TOK_TYPE)
    {
        Token *param_type = expect(stream, TOK_TYPE);
        Token *param_name = expect(stream, TOK_IDENT);

        TypeSpecifier param_ts = symtab_get(table, param_type->value)->type;
        symtab_add(table, make_symtab_entry(param_name->value, param_ts, VARIABLE));

        vector_push(params, param_name->value);

        if (current_token(stream)->type == TOK_RPAREN)
        {
            break;
        }
        expect(stream, TOK_COMMA);
    }

    return params;
}

AstNode *parse_func_def(TokenStream *stream, SymTab *table)
{
    Token *func_type = expect(stream, TOK_TYPE);
    Token *func_name = expect(stream, TOK_IDENT);

    // Check if symbol exists in table
    if (symtab_get(table, func_name->value))
    {
        perror("Redefinition of function.");
        exit(1);
    }

    // Parse function args
    expect(stream, TOK_LPAREN);
    Vector *params = parse_func_params(stream, table);
    expect(stream, TOK_RPAREN);

    // Insert args into symbol table
    TypeSpecifier ret_type = symtab_get(table, func_type->value)->type;
    SymTabEntry *entry = make_symtab_entry(func_name->value, ret_type, FUNCTION);
    symtab_add(table, entry);

    // Parse function body
    expect(stream, TOK_LBRACE);
    Vector *body = parse_body(stream, table);
    expect(stream, TOK_RBRACE);
    return make_ast_func_def(func_name->value, body, params);
}

AstNode *parse_var_def(TokenStream *stream, SymTab *table)
{
    Token *var_type = expect(stream, TOK_TYPE);
    Token *var_name = expect(stream, TOK_IDENT);

    // Check if symbol exists in table
    if (symtab_get(table, var_name->value))
    {
        perror("Redefinition of variable.");
        exit(1);
    }

    // Parse init expression
    AstNode *init = NULL;
    if (current_token(stream)->type == TOK_ASSIGN)
    {
        next_token(stream);
        init = parse_expression(stream, table);
    }

    expect(stream, TOK_SEMICOLON);

    // Successfull parse add to table
    SymTabEntry *entry = make_symtab_entry(var_name->value, symtab_get(table, var_type->value)->type, VARIABLE);
    symtab_add(table, entry);

    return make_ast_var_def(var_name->value, init);
}

// Returns root AstNode representing program
AstNode *parse(Vector *tokens, SymTab *table)
{
    TokenStream *stream = make_token_stream(tokens);
    AstNode *root = parse_statement(stream, table);
    free(stream);
    return root;
}
