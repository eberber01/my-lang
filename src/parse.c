// #include "ast.h"
#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ast.h"
#include "lex.h"
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

    fprintf(stderr, "Expected token of type %d at line: %d, pos: %d, actual type: %d\n", expect, current->line,
            current->pos, current->type);
    exit(1);
}

AstNode *parse_primary_expression(TokenStream *stream)
{

    Token *current = current_token(stream);
    if (current->type == TOK_NUM)
    {
        int lit;
        next_token(stream);
        char *int_str = as_str(current->value);
        str2int(&lit, int_str, 10);
        free(int_str);
        return make_int_const(lit);
    }
    else if (current->type == TOK_IDENT)
    {
        char *value = as_str(current->value);
        if (is_func_call_start(stream))
        {
            free(value);
            return parse_func_call(stream);
        }
        next_token(stream);
        return make_ast_ident(value);
    }
    else if (current->type == TOK_LPAREN)
    {
        next_token(stream);
        AstNode *ret = parse_expression(stream);

        expect(stream, TOK_RPAREN);

        return ret;
    }
    else
    {
        return make_ast_node(AST_EMPTY_EXPR, NULL);
    }
}

AstNode *parse_postfix_expression(TokenStream *stream)
{
    return parse_primary_expression(stream);
}

AstNode *parse_unary_expression(TokenStream *stream)
{
    return parse_postfix_expression(stream);
}

AstNode *parse_cast_expression(TokenStream *stream)
{
    return parse_postfix_expression(stream);
}

AstNode *parse_multiplicative_expression(TokenStream *stream)
{
    AstNode *left = parse_cast_expression(stream);
    Token *current;

    while ((current = current_token(stream)) && (current->type == TOK_MULT || current->type == TOK_DIV))
    {
        char *value = as_str(current->value);
        TokenType op_type = current->type;
        next_token(stream);
        AstNode *right = parse_cast_expression(stream);
        left = make_ast_bin_exp(value, op_type, left, right);
    }
    return left;
}

AstNode *parse_additive_expression(TokenStream *stream)
{
    AstNode *left = parse_multiplicative_expression(stream);
    Token *current;

    while ((current = current_token(stream)) && (current->type == TOK_ADD || current->type == TOK_SUB))
    {
        char *value = as_str(current->value);
        TokenType op_type = current->type;
        next_token(stream);
        AstNode *right = parse_multiplicative_expression(stream);
        left = make_ast_bin_exp(value, op_type, left, right);
    }
    return left;
}

AstNode *parse_shift_expression(TokenStream *stream)
{
    return parse_additive_expression(stream);
}

AstNode *parse_relational_expression(TokenStream *stream)
{
    return parse_shift_expression(stream);
}

AstNode *parse_equality_expression(TokenStream *stream)
{
    return parse_relational_expression(stream);
}

AstNode *parse_conditional_expression(TokenStream *stream)
{

    AstNode *left = parse_relational_expression(stream);
    Token *current;

    while ((current = current_token(stream)) && (current->type == TOK_NOT_EQUAL || current->type == TOK_EQUAL))
    {
        char *value = as_str(current->value);
        TokenType op_type = current->type;

        next_token(stream);
        AstNode *right = parse_relational_expression(stream);
        left = make_ast_bin_exp(value, op_type, left, right);
    }
    return left;
}

AstNode *parse_expression(TokenStream *stream)
{
    if (is_var_asgn(stream))
        return parse_var_asgn(stream);

    return parse_conditional_expression(stream);
}

int is_func_call_start(TokenStream *stream)
{
    Token *func_name = current_token(stream);
    Token *lparen = peek(stream, 1);
    return func_name->type == TOK_IDENT && lparen->type == TOK_LPAREN;
}

AstNode *parse_func_call(TokenStream *stream)
{
    Token *name = expect(stream, TOK_IDENT);
    char *name_str = as_str(name->value);
    expect(stream, TOK_LPAREN);

    Vector *args = vector_new();
    while (current_token(stream)->type != TOK_RPAREN)
    {

        AstNode *expr = parse_expression(stream);

        vector_push(args, expr);

        if (current_token(stream)->type == TOK_RPAREN)
        {
            break;
        }

        expect(stream, TOK_COMMA);
    }
    expect(stream, TOK_RPAREN);

    return make_ast_func_call(name_str, args);
}

int is_func_def_start(TokenStream *stream)
{
    Token *func_type = current_token(stream);
    Token *func_name = peek(stream, 1);
    Token *paren = peek(stream, 2);
    return func_type->type == TOK_TYPE && func_name->type == TOK_IDENT && paren->type == TOK_LPAREN;
}

bool is_var_def_start(TokenStream *stream)
{
    Token *var_type = current_token(stream);
    Token *var_name = peek(stream, 1);
    Token *assign = peek(stream, 2);
    return var_type->type == TOK_TYPE && var_name->type == TOK_IDENT && assign->type == TOK_ASSIGN;
}

bool is_var_dec(TokenStream *stream)
{
    Token *var_type = current_token(stream);
    Token *var_name = peek(stream, 1);
    Token *semi = peek(stream, 2);
    return var_type->type == TOK_TYPE && var_name->type == TOK_IDENT && semi->type == TOK_SEMICOLON;
}

bool is_var_asgn(TokenStream *stream)
{
    Token *var_name = current_token(stream);
    Token *asgn = peek(stream, 1);
    return var_name->type == TOK_IDENT && asgn->type == TOK_ASSIGN;
}

AstNode *parse_if_else_statement(TokenStream *stream)
{
    AstNode *else_body = NULL;
    expect(stream, TOK_IF);
    expect(stream, TOK_LPAREN);
    AstNode *expr = parse_conditional_expression(stream);

    expect(stream, TOK_RPAREN);

    AstNode *if_body = parse_statement(stream);
    if (current_token(stream)->type == TOK_ELSE)
    {
        expect(stream, TOK_ELSE);
        else_body = parse_statement(stream);
    }

    return make_ast_if_else(expr, if_body, else_body);
}

AstNode *parse_enum(TokenStream *stream)
{
    Token *curr;
    Token *enum_ident;
    Token *parent_enum;
    Vector *enums = vector_new();

    expect(stream, TOK_ENUM);
    parent_enum = expect(stream, TOK_IDENT);
    expect(stream, TOK_LBRACE);

    while ((curr = current_token(stream)) && curr->type != TOK_RBRACE)
    {
        enum_ident = expect(stream, TOK_IDENT);

        vector_push(enums, as_str(enum_ident->value));

        if ((curr = current_token(stream)) && curr->type != TOK_COMMA)
        {
            break;
        }

        expect(stream, TOK_COMMA);
    }
    expect(stream, TOK_RBRACE);
    expect(stream, TOK_SEMICOLON);

    return make_ast_enum(as_str(parent_enum->value), enums);
}

AstNode *parse_comp_stmt(TokenStream *stream)
{

    Token *current;
    AstNode *stmt;
    Vector *body = vector_new();

    expect(stream, TOK_LBRACE);
    while ((current = current_token(stream)) && current_token(stream)->type != TOK_RBRACE)
    {
        stmt = parse_statement(stream);
        vector_push(body, stmt);
    }
    expect(stream, TOK_RBRACE);
    return make_ast_comp_stmt(body);
}

AstNode *parse_while(TokenStream *stream)
{
    AstNode *expr;
    AstNode *body;

    expect(stream, TOK_WHILE);
    expect(stream, TOK_LPAREN);

    expr = parse_expression(stream);

    expect(stream, TOK_RPAREN);

    body = parse_statement(stream);

    return make_ast_while(expr, body);
}

AstNode *parse_for(TokenStream *stream)
{
    AstNode *init;
    expect(stream, TOK_FOR);
    expect(stream, TOK_LPAREN);
    if (is_declartion(stream))
    {

        init = parse_declartion(stream);
    }
    else
    {
        init = parse_expression(stream);
        expect(stream, TOK_SEMICOLON);
    }

    AstNode *cond = parse_expression(stream);
    expect(stream, TOK_SEMICOLON);

    AstNode *step = parse_expression(stream);
    expect(stream, TOK_RPAREN);

    AstNode *body = parse_statement(stream);
    return make_ast_for(init, cond, step, body);
}

AstNode *parse_expression_statement(TokenStream *stream)
{

    AstNode *expr = parse_expression(stream);
    expect(stream, TOK_SEMICOLON);
    return make_expr_stmt(expr);
}

AstNode *parse_iter_statement(TokenStream *stream)
{
    Token *current = current_token(stream);
    if (current->type == TOK_FOR)
        return parse_for(stream);

    return parse_while(stream);
}

AstNode *parse_return(TokenStream *stream)
{
    expect(stream, TOK_RETURN);
    AstNode *expr = parse_expression(stream);

    AstNode *ret_node = make_ast_ret(expr);
    expect(stream, TOK_SEMICOLON);
    return ret_node;
}

AstNode *parse_jump_statement(TokenStream *stream)
{
    return parse_return(stream);
}

AstNode *parse_selection_statement(TokenStream *stream)
{
    return parse_if_else_statement(stream);
}

bool is_declartion(TokenStream *stream)
{
    return is_var_dec(stream) || is_var_def_start(stream);
}

AstNode *parse_declartion(TokenStream *stream)
{
    if (is_var_def_start(stream))
        return parse_var_def(stream);
    else
        return parse_var_dec(stream);
}

AstNode *parse_statement(TokenStream *stream)
{
    Token *current = current_token(stream);
    
    if (current->type == TOK_WHILE || current->type == TOK_FOR)
        return parse_iter_statement(stream);
    else if (is_declartion(stream))
        return parse_declartion(stream);
    else if (current->type == TOK_RETURN)
        return parse_jump_statement(stream);
    else if (current->type == TOK_IF)
        return parse_selection_statement(stream);
    else if (current->type == TOK_ENUM)
        return parse_enum(stream);
    else if (current->type == TOK_LBRACE)
        return parse_comp_stmt(stream);
    else
        return parse_expression_statement(stream);
}

Vector *parse_func_params(TokenStream *stream)
{
    Vector *params = vector_new();
    Token *current;
    while ((current = current_token(stream)) && current_token(stream)->type == TOK_TYPE)
    {
        Token *param_type = expect(stream, TOK_TYPE);
        Token *param_name = expect(stream, TOK_IDENT);

        char *ts_str = as_str(param_type->value);

        Param *param = (Param *)my_malloc(sizeof(Param));
        param->type = ts_str;
        param->value = as_str(param_name->value);
        vector_push(params, param);

        if (current_token(stream)->type == TOK_RPAREN)
        {
            break;
        }
        expect(stream, TOK_COMMA);
    }

    return params;
}

AstNode *parse_func_def(TokenStream *stream)
{
    Token *func_type = expect(stream, TOK_TYPE);
    Token *func_name = expect(stream, TOK_IDENT);

    char *func_str = as_str(func_name->value);

    // Parse function args
    expect(stream, TOK_LPAREN);
    Vector *params = parse_func_params(stream);
    expect(stream, TOK_RPAREN);

    char *ts_name = as_str(func_type->value);

    // Parse function body
    AstNode *body = parse_comp_stmt(stream);
    return make_ast_func_def(func_str, ts_name, body, params);
}

AstNode *parse_var_dec(TokenStream *stream)
{
    Token *dec_type = expect(stream, TOK_TYPE);
    Token *dec_value = expect(stream, TOK_IDENT);

    char *value = as_str(dec_value->value);
    char *type = as_str(dec_type->value);

    expect(stream, TOK_SEMICOLON);
    return make_ast_var_dec(value, type);
}

AstNode *parse_var_asgn(TokenStream *stream)
{
    Token *asgn_ident = expect(stream, TOK_IDENT);
    char *value = as_str(asgn_ident->value);
    expect(stream, TOK_ASSIGN);

    AstNode *expr = parse_expression(stream);

    return make_ast_var_asgn(value, expr);
}

AstNode *parse_var_def(TokenStream *stream)
{
    Token *var_type = expect(stream, TOK_TYPE);
    Token *var_name = expect(stream, TOK_IDENT);
    char *var_str = as_str(var_name->value);
    char *type_str = as_str(var_type->value);

    // Parse init expression
    AstNode *init = NULL;
    if (current_token(stream)->type == TOK_ASSIGN)
    {
        next_token(stream);
        init = parse_expression(stream);
    }
    expect(stream, TOK_SEMICOLON);

    return make_ast_var_def(var_str, type_str, init);
}

Vector *parse_prog(TokenStream *stream)
{
    Token *curr;
    AstNode *stmt;
    Vector *prog = vector_new();
    while ((curr = current_token(stream)))
    {

        stmt = parse_func_def(stream);
        vector_push(prog, stmt);
    }
    return prog;
}

// Returns list of AstNode representing program
Vector *parse(Vector *tokens)
{
    TokenStream *stream = make_token_stream(tokens);
    Vector *prog = parse_prog(stream);
    free(stream);
    return prog;
}
