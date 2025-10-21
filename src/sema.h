#ifndef SEMA_H
#define SEMA_H

#include "ast.h"
#include "symtab.h"

typedef struct Scope Scope;

struct Scope
{
    Scope *parent;
    SymTab *table;
    int id;
};

void sym_check(AstNode *node, StackFrame *frame, Scope *scope);

void sema_check(Vector *prog, SymTab *table);

#endif
