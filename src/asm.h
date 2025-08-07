#ifndef ASM_H
#define ASM_H

#include <stdbool.h>
#include <stdio.h>
#include "ast.h"
#include "symtab.h"
#include "util.h"

#define REGISTER_SIZE 4
#define INT_SIZE 4

typedef struct Register
{
    // Name of register
    char *label;
    // Loaded value
    int value;

    bool free;
} Register;

typedef struct RISCV
{
    // Temporary registers
    Vector *temp;
    // Argument registers
    Vector *arg;
    // Saved register
    Vector *save;
    // Return register
    Register *ret;
    // Stack Pointer
    Register *sp;
    // Assembly file
    FILE *out;
} RISCV;

Register *alloc_register(RISCV *_asm);

void free_register(Register *reg);

Register *reg_add(Register *reg1, Register *reg2, RISCV *_asm);

Register *reg_sub(Register *reg1, Register *reg2, RISCV *_asm);

Register *reg_mult(Register *, Register *reg2, RISCV *_asm);

Register *reg_div(Register *, Register *reg2, RISCV *_asm);

Register *load_register(Register *reg, int value, RISCV *_asm);

Register *asm_eval(AstNode *node, SymTab *table, StackFrame *frame, RISCV *riscv);

void gen_asm(Vector *prog, SymTab *table);

RISCV *make_riscv(void);

#endif
