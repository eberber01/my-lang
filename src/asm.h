#ifndef ASM_H
#define ASM_H
#include "ast.h" 
#include "symtab.h"
#include<stdio.h>
int alloc_register();

void free_register(int reg);
int reg_add(int a, int b, FILE* out);
int reg_sub(int a, int b, FILE* out);
int reg_mult(int a, int b, FILE* out);
int reg_div(int a, int b, FILE* out);

int load_register(int reg, int value, FILE* out);

int asm_eval(AstNode* node,SymTab* table, FILE* out);

void gen_asm(AstNode* root, SymTab* table);


#endif