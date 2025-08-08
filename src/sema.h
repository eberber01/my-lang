#ifndef SEMA_H
#define SEMA_H

#include "symtab.h"

typedef struct Scope Scope;

struct Scope {
    Scope* parent;
    SymTab* table;
};

void sym_check(AstNode* node, SymTab *table);

void sema_check(Vector *prog, SymTab *table);

#endif
