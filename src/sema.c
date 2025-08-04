#include "symtab.h"
#include "ast.h"

typedef struct Scope Scope;

struct Scope {
    Scope* parent;
    SymTab* table;
};

void sym_check(AstNode* node, Scope* scope){
    SymTabEntry *entry;
    switch (node->type)
    {
    case AST_COMP_STMT:
        //Create new scope
        break;
    case AST_FUNC_DEF:
        AstFuncDef* func_def = (AstFuncDef*)node->as;
        entry = symtab_get(scope->table, func_def->value);

        if(entry){
            perror("Redefined function.");
        }

    default:
        break;
    }

}

void sema_check(Vector *prog){
    AstNode* node;
    for(int i=0; i < prog->length; i++)
    {
        node = (AstNode*)(prog, i);
        sym_check(node);
    }

}