#ifndef ASM_H
#define ASM_H

#include "ast.h"
#include "hashmap.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define REGISTER_SIZE 4

typedef char *Label;

typedef enum LabelKind
{
    LBL_FOR,
    LBL_IF,
    LBL_WHILE,
    LBL_ELSE,
    LBL_LOG_AND,
    LBL_LOG_OR
} LabelKind;

typedef struct Register
{
    // Name of register
    char *label;
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
    size_t for_count;
    size_t while_count;
    size_t if_count;
    size_t else_count;
    size_t and_count;
    size_t or_count;
} RISCV;

Register *alloc_register(RISCV *_asm);

void free_register(Register *reg);

Register *emit_reg_add(Register *reg1, Register *reg2, RISCV *_asm);

Register *emit_reg_sub(Register *reg1, Register *reg2, RISCV *_asm);

Register *emit_reg_mult(Register *, Register *reg2, RISCV *_asm);

Register *emit_reg_div(Register *, Register *reg2, RISCV *_asm);

Register *emit_load_register(Register *reg, int value, RISCV *_asm);

Register *eval_asm(AstNode *node, RISCV *_asm);

void gen_asm(Vector *prog);

void _gen_asm(AstNode *node, RISCV *_asm);

RISCV *make_riscv(void);

void ecall(RISCV *_asm);

#endif
