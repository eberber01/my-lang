#include "asm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "hashmap.h"
#include "util.h"

int cond_count = 0;

Register *make_register(char *label)
{
    Register *reg = my_malloc(sizeof(Register));
    reg->label = label;
    reg->free = true;
    return reg;
}

RISCV *make_riscv(void)
{
    RISCV *riscv = my_malloc(sizeof(RISCV));
    Vector *temp_reg = vector_new();
    Vector *arg_reg = vector_new();
    Vector *save_reg = vector_new();

    // Temp registers
    char *temp[7] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};
    char *arg[8] = {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};
    char *save[12] = {"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11"};
    for (int i = 0; i < 7; i++)
    {
        vector_push(temp_reg, make_register(temp[i]));
    }
    for (int i = 0; i < 8; i++)
    {
        vector_push(arg_reg, make_register(arg[i]));
    }
    for (int i = 0; i < 12; i++)
    {
        vector_push(save_reg, make_register(save[i]));
    }
    riscv->temp = temp_reg;
    riscv->arg = arg_reg;
    riscv->save = save_reg;
    riscv->ret = make_register("ra");
    riscv->sp = make_register("sp");
    riscv->sp->value = 0;
    return riscv;
}

void sp_increase(size_t bytes, RISCV *_asm)
{
    fprintf(_asm->out, "\taddi sp, sp, -%zu\n", bytes);
    _asm->sp->value += bytes;
}

void sp_decrease(size_t bytes, RISCV *_asm)
{
    fprintf(_asm->out, "\taddi sp, sp, %zu\n", bytes);
    _asm->sp->value -= bytes;
}

// Load value at sp offset into reg
Register *sp_load(Register *reg, int offset, RISCV *_asm)
{
    fprintf(_asm->out, "\tlw %s, %d(sp)\n", reg->label, offset);
    return reg;
}

// Store value at reg at sp plus offset
void sp_store(int offset, Register *reg, RISCV *_asm)
{
    fprintf(_asm->out, "\tsw %s, %d(sp)\n", reg->label, offset);
}

void print_newline(RISCV *_asm)
{
    fprintf(_asm->out, "\tli a7, 11\n"); // service 11 is print integer
    Register *r = alloc_register(_asm);
    load_register(r, 10, _asm);
    fprintf(_asm->out, "\tadd a0, %s, zero\n",
            r->label); // load desired value into argument register a0, using
                       // pseudo-op
    fprintf(_asm->out, "\tecall\n");
    free_register(r);
}

void print_register(Register *reg, RISCV *_asm)
{
    fprintf(_asm->out, "\tli  a7, 1\n"); // service 1 is print integer
    fprintf(_asm->out, "\tadd a0, %s, zero\n",
            reg->label); // load desired value into argument register a0, using
                         // pseudo-op
    fprintf(_asm->out, "\tecall\n");
}

// Allocate free temporary register
Register *alloc_register(RISCV *_asm)
{
    for (size_t i = 0; i < _asm->temp->length; i++)
    {
        Register *reg = vector_get(_asm->temp, i);
        if (reg->free)
        {
            reg->free = false;
            return reg;
        }
    }
    perror("No free registers");
    exit(1);
}

void free_register(Register *reg)
{
    reg->free = true;
}

// Register Addition
// Free's Reg2
Register *reg_add(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tadd %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Subtraction
// Free's Reg2
Register *reg_sub(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tsub %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Multiplication
//  Free's Reg2
Register *reg_mult(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tmul %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Division
//  Free's Reg2
Register *reg_div(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tdiv %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Load register with int value
Register *load_register(Register *reg, int value, RISCV *_asm)
{
    fprintf(_asm->out, "\taddi %s, zero, %d\n", reg->label, value);
    return reg;
}

void jump_to_label(char *label, RISCV *_asm)
{
    fprintf(_asm->out, "\tjal %s\n", label);
}

void return_from_jump(RISCV *_asm)
{
    fprintf(_asm->out, "\tjalr ra\n");
}

// Add Label to RISCV out file
// name:
void label_add(char *name, RISCV *_asm)
{
    fprintf(_asm->out, "%s:\n", name);
}
// Move value from reg1 to reg2
void move_register(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tmv %s, %s\n", reg1->label, reg2->label);
}

Register *eval_func_call(AstNode *node, RISCV *_asm)
{
    AstFuncCall *func_call;
    Register *reg;

    func_call = (AstFuncCall *)node->as;

    // Allocate enough space for ra + temp and arg registers
    size_t stack_space = REGISTER_SIZE * (_asm->temp->length + _asm->arg->length + 1);

    size_t offset = 0;

    // Allocate space for return address
    sp_increase(stack_space, _asm);
    sp_store(offset, _asm->ret, _asm);
    offset += REGISTER_SIZE;

    // Save arg registers
    for (size_t i = 0; i < _asm->arg->length; i++)
    {
        reg = vector_get(_asm->arg, i);
        sp_store(offset, reg, _asm);
        offset += REGISTER_SIZE;
    }

    // Save temp registers
    int freed[7] = {0};
    for (size_t i = 0; i < _asm->temp->length; i++)
    {
        reg = vector_get(_asm->temp, i);
        if (!reg->free)
        {

            sp_store(offset, reg, _asm);
            offset += REGISTER_SIZE;
            freed[i] = 1;
            free_register(reg);
        }
    }

    // Eval func args and move them into arg register
    for (size_t i = 0; i < func_call->args->length; i++)
    {
        AstNode *arg = (AstNode *)vector_get(func_call->args, i);
        // This is always going to be an expression
        reg = asm_eval(arg, _asm);
        move_register(vector_get(_asm->arg, i), reg, _asm);
        free_register(reg);
    }

    // TODO move left over args to stack

    jump_to_label(func_call->value, _asm);

    // Restore temp registers
    for (int i = _asm->temp->length - 1; i >= 0; i--)
    {
        if (freed[i])
        {
            reg = vector_get(_asm->temp, i);
            offset -= REGISTER_SIZE;
            sp_load(reg, offset, _asm);
            reg->free = false;
        }
    }

    // Move return value to empty register
    reg = alloc_register(_asm);
    fprintf(_asm->out, "\tadd %s, zero, a0\n", reg->label);

    // Restore arg registers
    for (int i = _asm->arg->length - 1; i >= 0; i--)
    {

        offset -= REGISTER_SIZE;
        sp_load(vector_get(_asm->arg, i), offset, _asm);
    }

    offset -= REGISTER_SIZE;
    // restore return address
    sp_load(_asm->ret, offset, _asm);
    sp_decrease(stack_space, _asm);

    // Return register with return value
    return reg;
}

Register *eval_bin_exp(AstNode *node, RISCV *_asm)
{
    AstBinExp *bin_exp;
    Register *left;
    Register *right;
    Register *reg;

    bin_exp = (AstBinExp *)node->as;
    left = asm_eval(bin_exp->left, _asm);
    right = asm_eval(bin_exp->right, _asm);
    switch (*(bin_exp->value))
    {
    case '+':
        return reg_add(left, right, _asm);
    case '-':
        return reg_sub(left, right, _asm);
    case '*':
        return reg_mult(left, right, _asm);
    case '/':
        return reg_div(left, right, _asm);
    default:

        if (!strcmp(bin_exp->value, "==") || !strcmp(bin_exp->value, "!="))
        {
            reg = alloc_register(_asm);

            fprintf(_asm->out, "\tsub %s, %s, %s \n", reg->label, right->label, left->label);
            if (!strcmp(bin_exp->value, "=="))
            {

                // == Check if equal
                fprintf(_asm->out, "\tseqz %s, %s \n", reg->label, reg->label);
            }
            else
            {
                // !=
                fprintf(_asm->out, "\tsnez %s, %s \n", reg->label, reg->label);
            }

            return reg;
        }

        perror("Unexpected node type.");
        exit(1);
    }
}

Register *eval_if(AstNode *node, RISCV *_asm)
{
    AstIf *if_stmt;
    Register *reg;
    int id = cond_count;

    if_stmt = (AstIf *)node->as;
    // Add label
    fprintf(_asm->out, "if_start%d:\n", id);

    // eval bool expression, reg = 0 for false and reg = 1 for true
    reg = asm_eval(if_stmt->expr, _asm);

    // cmp and branch to end label
    fprintf(_asm->out, "\tbeq %s, zero, if_end%d\n", reg->label, id);

    // eval body
    asm_eval(if_stmt->body, _asm);

    // add other label
    fprintf(_asm->out, "if_end%d:\n", id);
    free_register(reg);
    cond_count++;
    return NULL;
}

Register *eval_ret(AstNode *node, RISCV *_asm)
{
    AstRet *ret;
    Register *reg;

    ret = (AstRet *)node->as;
    // return val
    reg = asm_eval(ret->expr, _asm);

    // Exit status 0, if main function
    if (!strcmp(ret->func, "main"))
    {
        // Syscall Exit 10
        fprintf(_asm->out, "\tli  a7, 93\n");
        move_register(reg, vector_get(_asm->arg, 0), _asm);
        ecall(_asm);
    }
    else
    {
        // move to return a0
        fprintf(_asm->out, "\tadd a0, %s, zero\n", reg->label);
    }
    free_register(reg);
    return NULL;
}

Register *eval_comp_stmt(AstNode *node, RISCV *_asm)
{
    AstCompStmt *comp_stmt;
    comp_stmt = (AstCompStmt *)node->as;

    for (size_t i = 0; i < comp_stmt->body->length; i++)
    {
        AstNode *stmt = (AstNode *)vector_get(comp_stmt->body, i);
        asm_eval(stmt, _asm);
    }
    return NULL;
}

Register *eval_int_const(AstNode *node, RISCV *_asm)
{
    Register *reg;
    AstIntConst *int_node;
    int_node = (AstIntConst *)node->as;

    reg = alloc_register(_asm);
    load_register(reg, int_node->value, _asm);
    return reg;
}

Register *eval_func_def(AstNode *node, RISCV *_asm)
{
    AstFuncDef *func_def;
    func_def = (AstFuncDef *)node->as;
    label_add(func_def->value, _asm);

    if (func_def->params->length > 8)
    {
        perror("Max params reached");
    }
    for (size_t i = 0; i < func_def->params->length; i++)
    {
        Param *param = (Param *)vector_get(func_def->params, i);
        param->symbol->is_arg_loaded = true;
        param->symbol->arg_reg = i;
    }

    size_t frame_size = func_def->symbol->frame->size;
    // Create stack space for frame

    if (frame_size > 0)
        sp_increase(frame_size, _asm);

    asm_eval(func_def->body, _asm);

    if (frame_size > 0)
        sp_increase(frame_size, _asm);

    // Not main function, return
    if (strcmp(func_def->value, "main"))
    {
        return_from_jump(_asm);
    }

    return NULL;
}

Register *eval_var_def(AstNode *node, RISCV *_asm)
{
    int offset;
    AstVarDef *var_def;
    Register *reg;

    var_def = (AstVarDef *)node->as;
    // Increase stack frame size
    offset = var_def->symbol->offset;

    // store value on to stack
    reg = asm_eval(var_def->expr, _asm);
    sp_store(offset, reg, _asm);
    free_register(reg);

    return NULL;
}

Register *eval_ident(AstNode *node, RISCV *_asm)
{
    AstIdent *ident_node;
    Register *reg;
    SymTabEntry *var;

    // Store location in symbol table
    ident_node = (AstIdent *)node->as;
    var = ident_node->symbol;

    if (var->symbol == SYM_CONST)
    {
        reg = alloc_register(_asm);
        load_register(reg, var->const_value, _asm);
        return reg;
    }
    if (var->is_arg_loaded)
    {
        return (Register *)vector_get(_asm->arg, var->arg_reg);
    }

    // Load value from sp
    reg = alloc_register(_asm);
    sp_load(reg, var->offset, _asm);
    return reg;
}

Register *eval_var_asgn(AstNode *node, RISCV *_asm)
{
    int offset;
    AstVarAsgn *asgn;
    Register *reg;

    asgn = (AstVarAsgn *)node->as;

    offset = asgn->symbol->offset;

    reg = asm_eval(asgn->expr, _asm);

    sp_store(offset, reg, _asm);

    return reg;
}

Register *eval_while(AstNode *node, RISCV *_asm)
{
    AstWhile *w_stmt = (AstWhile *)node->as;
    Register *reg;
    int id = cond_count;
    // Add label
    fprintf(_asm->out, "while_start%d:\n", id);

    reg = asm_eval(w_stmt->expr, _asm);
    // cmp and branch to end label
    fprintf(_asm->out, "\tbeq %s, zero, while_end%d\n", reg->label, id);

    free_register(reg);

    asm_eval(w_stmt->body, _asm);

    // Jump back to start
    fprintf(_asm->out, "\tj while_start%d\n", id);

    // add other label
    fprintf(_asm->out, "while_end%d:\n", id);

    cond_count++;
    return NULL;
}

Register *eval_for(AstNode *node, RISCV *_asm)
{
    AstFor *f_stmt = (AstFor *)node->as;
    Register *reg;
    int id = cond_count;

    // Add label
    fprintf(_asm->out, "for_init%d:\n", id);
    reg = asm_eval(f_stmt->init, _asm);

    // Could be expression or declartion so we need to free register
    if (reg != NULL)
        free_register(reg);

    fprintf(_asm->out, "for_cond%d:\n", id);
    reg = asm_eval(f_stmt->cond, _asm);

    if (reg != NULL)
    {
        // cmp and branch to end label
        fprintf(_asm->out, "\tbeq %s, zero, for_end%d\n", reg->label, id);
        free_register(reg);
    }

    fprintf(_asm->out, "for_body%d:\n", id);
    asm_eval(f_stmt->body, _asm);

    fprintf(_asm->out, "for_step%d:\n", id);
    reg = asm_eval(f_stmt->step, _asm);
    if (reg != NULL)
    {
        free_register(reg);
    }

    fprintf(_asm->out, "\tj for_cond%d\n", id);
    fprintf(_asm->out, "for_end%d:\n", id);

    cond_count++;
    return NULL;
}

// Recursively write AST representation to Assembly file
Register *asm_eval(AstNode *node, RISCV *_asm)
{
    switch (node->type)
    {
    case AST_FUNC_CALL:
        return eval_func_call(node, _asm);
    case AST_IF:
        return eval_if(node, _asm);
    case AST_RET:
        return eval_ret(node, _asm);
    case AST_COMP_STMT:
        return eval_comp_stmt(node, _asm);
    case AST_INT_CONST:
        return eval_int_const(node, _asm);
    case AST_FUNC_DEF:
        return eval_func_def(node, _asm);
    case AST_VAR_DEF:
        return eval_var_def(node, _asm);
    case AST_IDENT:
        return eval_ident(node, _asm);
    case AST_BIN_EXP:
        return eval_bin_exp(node, _asm);
    case AST_VAR_ASGN:
        return eval_var_asgn(node, _asm);
    case AST_WHILE:
        return eval_while(node, _asm);
    case AST_FOR:
        return eval_for(node, _asm);
    case AST_ENUM:
    case AST_EMPTY_EXPR:
    case AST_VAR_DEC:
        return NULL;
    default:
        perror("unkown ast type");
        exit(1);
    }
}

void asm_init(RISCV *riscv)
{
    // Create and set up out file
    FILE *out = fopen("./asm.s", "w");
    fprintf(out, ".globl main\n\n");
    fprintf(out, "\tj main\n");
    riscv->out = out;
}

void asm_free(RISCV *riscv)
{
    fclose(riscv->out);
    for (int i = 0; i < 7; i++)
        free(vector_get(riscv->temp, i));
    for (int i = 0; i < 8; i++)
        free(vector_get(riscv->arg, i));
    for (int i = 0; i < 12; i++)
        free(vector_get(riscv->save, i));
    vector_free(riscv->arg);
    vector_free(riscv->temp);
    vector_free(riscv->save);
    free(riscv->sp);
    free(riscv->ret);
    free(riscv);
}

void ecall(RISCV *_asm)
{
    fprintf(_asm->out, "\tecall\n");
}

// Generate Assembly file from AST
void gen_asm(Vector *prog)
{
    int linux_exit_syscall = 93;
    int exit_code = 0;

    RISCV *_asm = make_riscv();
    asm_init(_asm);
    for (size_t i = 0; i < prog->length; i++)
    {
        asm_eval((AstNode *)vector_get(prog, i), _asm);
    }

    // Linux prologue

    // a7
    load_register(vector_get(_asm->arg, 7), linux_exit_syscall, _asm);

    // a0
    load_register(vector_get(_asm->arg, 0), exit_code, _asm);
    ecall(_asm);

    asm_free(_asm);
}
