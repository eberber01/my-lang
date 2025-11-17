#ifndef SEMA_H
#define SEMA_H

#include "ast.h"
#include "hashmap.h"

typedef struct Scope Scope;

struct Scope
{
    Scope *parent;
    HashMap *symtab;
    int id;
};

void sym_check(AstNode *node, StackFrame *frame, Scope *scope, HashMap *type_env);

void sema_check(Vector *prog, HashMap *type_env);

#endif
