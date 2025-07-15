#include <gtest/gtest.h>
extern "C"{
  #include "../parse.h"
  #include "../lex.h"
  #include "../symtab.h"
  #include "../util.h"
  #include "../ast.h"
}


TEST(Parse, Variable){
    Vector* v;
    TokenStream* s;
    SymTab* table;
    AstNode* var;

    v = vector_new();
    table = symtab_new();
    symtab_init(table);

    vector_push(v, make_token(TOK_TYPE, (char*)"int", 0,0));
    vector_push(v, make_token(TOK_IDENT, (char*)"var", 0,0));
    vector_push(v, make_token(TOK_ASSIGN, (char*)"=", 0,0));
    vector_push(v, make_token(TOK_NUM, (char*)"10", 0,0));

    s = make_token_stream(v);

    var = parse_variable(s, table);

    EXPECT_EQ(AST_VAR_DEF, var->type);

    symtab_free(table);
    vector_free(v);
    free(s);
    ast_free(var);
}