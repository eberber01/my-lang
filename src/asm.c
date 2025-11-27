#include "asm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "hashmap.h"
#include "lex.h"
#include "util.h"

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
    riscv->zero = make_register("zero");
    riscv->while_count = 0;
    riscv->for_count = 0;
    riscv->if_count = 0;
    riscv->else_count = 0;
    riscv->and_count = 0;
    riscv->or_count = 0;
    return riscv;
}

Label create_base_cond_label(LabelKind kind, RISCV *_asm)
{
    char tmp[64];
    switch (kind)
    {
    case LBL_FOR:
        snprintf(tmp, sizeof(tmp), "for%zu", _asm->for_count);
        _asm->for_count++;
        break;
    case LBL_IF:
        snprintf(tmp, sizeof(tmp), "if%zu", _asm->if_count);
        _asm->if_count++;
        break;
    case LBL_WHILE:
        snprintf(tmp, sizeof(tmp), "while%zu", _asm->while_count);
        _asm->while_count++;
        break;
    case LBL_ELSE:
        snprintf(tmp, sizeof(tmp), "else%zu", _asm->else_count);
        _asm->else_count++;
        break;
    case LBL_LOG_AND:
        snprintf(tmp, sizeof(tmp), "and%zu", _asm->and_count);
        _asm->and_count++;
        break;
        break;
    case LBL_LOG_OR:
        snprintf(tmp, sizeof(tmp), "or%zu", _asm->or_count);
        _asm->or_count++;
        break;
    }
    Label label = my_malloc(sizeof(char) * strlen(tmp) + 1);
    strcpy(label, tmp);
    return label;
}

Label extend_label(Label label, char *str)
{
    char tmp[64];
    snprintf(tmp, sizeof(tmp), "%s%s", label, str);

    Label new_label = my_malloc(sizeof(char) * strlen(tmp) + 1);
    strcpy(new_label, tmp);
    return new_label;
}

void emit_branch_eq(Register *reg1, Register *reg2, Label label, RISCV *_asm)
{
    fprintf(_asm->out, "\tbeq %s, %s, %s\n", reg1->label, reg2->label, label);
}

void emit_branch_not_eq(Register *reg1, Register *reg2, Label label, RISCV *_asm)
{
    fprintf(_asm->out, "\tbne %s, %s, %s\n", reg1->label, reg2->label, label);
}

void emit_jump_label(Label label, RISCV *_asm)
{
    fprintf(_asm->out, "\tj %s\n", label);
}

void emit_sp_increase(size_t bytes, RISCV *_asm)
{
    fprintf(_asm->out, "\taddi sp, sp, -%zu\n", bytes);
}

void emit_sp_decrease(size_t bytes, RISCV *_asm)
{
    fprintf(_asm->out, "\taddi sp, sp, %zu\n", bytes);
}

// Load value at sp offset into reg
Register *emit_sp_load(Register *reg, int offset, RISCV *_asm)
{
    fprintf(_asm->out, "\tlw %s, %d(sp)\n", reg->label, offset);
    return reg;
}

// Store value at reg at sp plus offset
void emit_sp_store(int offset, Register *reg, RISCV *_asm)
{
    fprintf(_asm->out, "\tsw %s, %d(sp)\n", reg->label, offset);
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
Register *emit_reg_add(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tadd %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Subtraction
// Free's Reg2
Register *emit_reg_sub(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tsub %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Multiplication
//  Free's Reg2
Register *emit_reg_mult(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tmul %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Division
//  Free's Reg2
Register *emit_reg_div(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tdiv %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Load register with int value
Register *emit_load_register(Register *reg, int value, RISCV *_asm)
{
    fprintf(_asm->out, "\taddi %s, zero, %d\n", reg->label, value);
    return reg;
}

void emit_jump_to_label(char *label, RISCV *_asm)
{
    fprintf(_asm->out, "\tjal %s\n", label);
}

void emit_return_from_jump(RISCV *_asm)
{
    fprintf(_asm->out, "\tjalr ra\n");
}

// Add Label to RISCV out file
// name:
void emit_label(char *name, RISCV *_asm)
{
    fprintf(_asm->out, "%s:\n", name);
}
// Move value from reg1 to reg2
void emit_move_register(Register *reg1, Register *reg2, RISCV *_asm)
{
    fprintf(_asm->out, "\tmv %s, %s\n", reg2->label, reg1->label);
}

Register *eval_func_call(AstNode *node, RISCV *_asm)
{
    AstFuncCall *func_call;
    Register *ret_reg;
    Register *reg;

    func_call = (AstFuncCall *)node->as;

    // Allocate enough space for ra + temp and arg registers
    size_t stack_space = REGISTER_SIZE * (_asm->temp->length + _asm->arg->length + 1);

    size_t offset = 0;

    // Allocate space for return address
    emit_sp_increase(stack_space, _asm);
    emit_sp_store(offset, _asm->ret, _asm);
    offset += REGISTER_SIZE;

    // Save arg registers
    for (size_t i = 0; i < _asm->arg->length; i++)
    {
        reg = vector_get(_asm->arg, i);
        emit_sp_store(offset, reg, _asm);
        offset += REGISTER_SIZE;
    }

    // Save temp registers
    int freed[7] = {0};
    for (size_t i = 0; i < _asm->temp->length; i++)
    {
        reg = vector_get(_asm->temp, i);
        if (!reg->free)
        {

            emit_sp_store(offset, reg, _asm);
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
        reg = eval_asm(arg, _asm);
        emit_move_register(reg, vector_get(_asm->arg, i), _asm);
        free_register(reg);
    }

    // TODO move left over args to stack

    emit_jump_to_label(func_call->value, _asm);

    // Restore temp registers
    for (int i = _asm->temp->length - 1; i >= 0; i--)
    {
        if (freed[i])
        {
            reg = vector_get(_asm->temp, i);
            offset -= REGISTER_SIZE;
            emit_sp_load(reg, offset, _asm);
            reg->free = false;
        }
    }

    // Move return value to empty register
    reg = alloc_register(_asm);
    ret_reg = (Register *)vector_get(_asm->arg, 0);

    emit_move_register(ret_reg, reg, _asm);

    // Restore arg registers
    for (int i = _asm->arg->length - 1; i >= 0; i--)
    {

        offset -= REGISTER_SIZE;
        emit_sp_load(vector_get(_asm->arg, i), offset, _asm);
    }

    offset -= REGISTER_SIZE;
    // restore return address
    emit_sp_load(_asm->ret, offset, _asm);
    emit_sp_decrease(stack_space, _asm);

    // Return register with return value
    return reg;
}

Register *eval_log_and(Register *left, Register *right, RISCV *_asm)
{

    Register *reg = alloc_register(_asm);
    Label and_label = create_base_cond_label(LBL_LOG_AND, _asm);
    Label false_label = extend_label(and_label, "false");
    Label end_label = extend_label(and_label, "end");

    emit_branch_eq(left, _asm->zero, false_label, _asm);
    emit_branch_eq(right, _asm->zero, false_label, _asm);

    emit_load_register(reg, 1, _asm);
    emit_jump_label(end_label, _asm);

    emit_label(false_label, _asm);
    emit_load_register(reg, 0, _asm);

    emit_label(end_label, _asm);

    free(and_label);
    free(false_label);
    free(end_label);

    free_register(left);
    free_register(right);

    return reg;
}

Register *eval_log_or(Register *left, Register *right, RISCV *_asm)
{

    Register *reg = alloc_register(_asm);

    Label or_label = create_base_cond_label(LBL_LOG_OR, _asm);
    Label true_label = extend_label(or_label, "true");
    Label end_label = extend_label(or_label, "end");

    emit_branch_not_eq(left, _asm->zero, true_label, _asm);
    emit_branch_not_eq(right, _asm->zero, true_label, _asm);

    emit_load_register(reg, 0, _asm);
    emit_jump_label(end_label, _asm);

    emit_label(true_label, _asm);

    emit_load_register(reg, 1, _asm);
    emit_label(end_label, _asm);

    free(or_label);
    free(true_label);
    free(end_label);

    free_register(left);
    free_register(right);
    return reg;
}

Register *eval_bin_exp(AstNode *node, RISCV *_asm)
{
    AstBinExp *bin_exp;
    Register *left;
    Register *right;
    Register *reg;
    Register *tmp;

    bin_exp = (AstBinExp *)node->as;
    left = eval_asm(bin_exp->left, _asm);
    right = eval_asm(bin_exp->right, _asm);
    switch (bin_exp->op_type)
    {
    case TOK_ADD:
        return emit_reg_add(left, right, _asm);
    case TOK_SUB:
        return emit_reg_sub(left, right, _asm);
    case TOK_MULT:
        return emit_reg_mult(left, right, _asm);
    case TOK_DIV:
        return emit_reg_div(left, right, _asm);
    case TOK_EQUAL:
        reg = alloc_register(_asm);

        fprintf(_asm->out, "\tsub %s, %s, %s \n", reg->label, right->label, left->label);
        // == Check if equal
        fprintf(_asm->out, "\tseqz %s, %s \n", reg->label, reg->label);
        free_register(left);
        free_register(right);
        return reg;
    case TOK_NOT_EQUAL:
        reg = alloc_register(_asm);

        fprintf(_asm->out, "\tsub %s, %s, %s \n", reg->label, right->label, left->label);
        // !=
        fprintf(_asm->out, "\tsnez %s, %s \n", reg->label, reg->label);

        free_register(left);
        free_register(right);
        return reg;
    case TOK_LOG_AND:
        return eval_log_and(left, right, _asm);
    case TOK_LOG_OR:
        return eval_log_or(left, right, _asm);
    case TOK_LT:
        reg = alloc_register(_asm);
        fprintf(_asm->out, "\tslt %s, %s, %s\n", reg->label, left->label, right->label);

        free_register(left);
        free_register(right);
        return reg;
    case TOK_LT_EQ:
        reg = alloc_register(_asm);
        tmp = alloc_register(_asm);

        // temp = rhs < lhs
        fprintf(_asm->out, "\tslt %s, %s, %s\n", tmp->label, right->label, left->label);

        // invert → lhs <= rhs
        fprintf(_asm->out, "\txori %s, %s, %d\n", reg->label, tmp->label, 1);

        free_register(left);
        free_register(right);
        free_register(tmp);
        return reg;
    case TOK_GT:
        reg = alloc_register(_asm);
        fprintf(_asm->out, "\tslt %s, %s, %s\n", reg->label, right->label, left->label);

        free_register(left);
        free_register(right);
        return reg;

    case TOK_GT_EQ:
        reg = alloc_register(_asm);
        tmp = alloc_register(_asm);

        // temp = lhs < rhs
        fprintf(_asm->out, "\tslt %s, %s, %s\n", tmp->label, left->label, right->label);

        // invert → lhs >= rhs
        fprintf(_asm->out, "\txori %s, %s, %d\n", reg->label, tmp->label, 1);

        free_register(left);
        free_register(right);
        free_register(tmp);
        return reg;
    case TOK_LSHIFT:
        reg = alloc_register(_asm);

        fprintf(_asm->out, "\tsll %s, %s, %s\n", reg->label, left->label, right->label);
        free_register(left);
        free_register(right);

        return reg;
    case TOK_RSHIFT:
        reg = alloc_register(_asm);

        // Signed bit shift
        fprintf(_asm->out, "\tsra %s, %s, %s\n", reg->label, left->label, right->label);
        free_register(left);
        free_register(right);

        return reg;
    case TOK_AND:
        reg = alloc_register(_asm);

        fprintf(_asm->out, "\tand %s, %s, %s\n", reg->label, left->label, right->label);
        free_register(left);
        free_register(right);

        return reg;
    case TOK_OR:
        reg = alloc_register(_asm);

        fprintf(_asm->out, "\tor %s, %s, %s\n", reg->label, left->label, right->label);
        free_register(left);
        free_register(right);

        return reg;
    case TOK_XOR:
        reg = alloc_register(_asm);

        fprintf(_asm->out, "\txor %s, %s, %s\n", reg->label, left->label, right->label);
        free_register(left);
        free_register(right);

        return reg;
    default:

        perror("Unexpected node type.");
        exit(1);
    }
}

void gen_if(AstNode *node, RISCV *_asm)
{
    AstIfElse *if_stmt;
    Register *reg;
    Label if_label = create_base_cond_label(LBL_IF, _asm);
    Label else_label = create_base_cond_label(LBL_ELSE, _asm);

    if_stmt = (AstIfElse *)node->as;
    // Add label
    emit_label(if_label, _asm);

    // eval bool expression, reg = 0 for false and reg = 1 for true
    reg = eval_asm(if_stmt->if_expr, _asm);

    // cmp and branch to end label
    fprintf(_asm->out, "\tbeq %s, zero, %s\n", reg->label, else_label);
    free_register(reg);

    // eval body
    _gen_asm(if_stmt->if_body, _asm);

    // add other label
    emit_label(else_label, _asm);

    if (if_stmt->else_body != NULL)
        _gen_asm(if_stmt->else_body, _asm);

    free(if_label);
    free(else_label);
}

void gen_ret(AstNode *node, RISCV *_asm)
{
    AstRet *ret;
    Register *reg;
    StackFrame *frame;

    ret = (AstRet *)node->as;
    frame = ret->func->frame;

    // return val
    if (ret->expr->type == AST_EMPTY_EXPR)
        _gen_asm(ret->expr, _asm);
    else
    {

        reg = eval_asm(ret->expr, _asm);

        // move to return a0
        fprintf(_asm->out, "\tadd a0, %s, zero\n", reg->label);
        free_register(reg);
    }

    // restore stack
    if (frame->size > 0)
        emit_sp_decrease(ret->func->frame->size, _asm);

    emit_return_from_jump(_asm);
}

void gen_comp_stmt(AstNode *node, RISCV *_asm)
{
    AstCompStmt *comp_stmt;
    comp_stmt = (AstCompStmt *)node->as;

    for (size_t i = 0; i < comp_stmt->body->length; i++)
    {
        AstNode *stmt = (AstNode *)vector_get(comp_stmt->body, i);
        _gen_asm(stmt, _asm);
    }
}

Register *eval_int_const(AstNode *node, RISCV *_asm)
{
    Register *reg;
    AstIntConst *int_node;
    int_node = (AstIntConst *)node->as;

    reg = alloc_register(_asm);
    emit_load_register(reg, int_node->value, _asm);
    return reg;
}

void gen_func_def(AstNode *node, RISCV *_asm)
{
    AstFuncDef *func_def;
    func_def = (AstFuncDef *)node->as;
    emit_label(func_def->value, _asm);

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
        emit_sp_increase(frame_size, _asm);

    _gen_asm(func_def->body, _asm);

    if (frame_size > 0)
        emit_sp_decrease(frame_size, _asm);

    emit_return_from_jump(_asm);
}

void gen_var_def(AstNode *node, RISCV *_asm)
{
    int offset;
    AstVarDef *var_def;
    Register *reg;

    var_def = (AstVarDef *)node->as;
    // Increase stack frame size
    offset = var_def->symbol->offset;

    // store value on to stack
    reg = eval_asm(var_def->expr, _asm);
    emit_sp_store(offset, reg, _asm);
    free_register(reg);
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
        emit_load_register(reg, var->const_value, _asm);
        return reg;
    }
    if (var->is_arg_loaded)
    {
        return (Register *)vector_get(_asm->arg, var->arg_reg);
    }

    // Load value from sp
    reg = alloc_register(_asm);
    emit_sp_load(reg, var->offset, _asm);
    return reg;
}

Register *eval_var_asgn(AstNode *node, RISCV *_asm)
{
    int offset;
    AstVarAsgn *asgn;
    Register *reg;

    asgn = (AstVarAsgn *)node->as;

    offset = asgn->symbol->offset;
    reg = eval_asm(asgn->expr, _asm);
    emit_sp_store(offset, reg, _asm);

    return reg;
}

Register *eval_unary_expr(AstNode *node, RISCV *_asm)
{
    Register *reg;
    Register *tmp;
    AstUnaryExpr *unary_expr;

    unary_expr = (AstUnaryExpr *)node->as;

    switch (unary_expr->op_type)
    {
    case TOK_NOT:
        reg = alloc_register(_asm);

        tmp = eval_asm(unary_expr->postfix_expr, _asm);
        fprintf(_asm->out, "\tseqz %s, %s\n", reg->label, tmp->label);

        free_register(tmp);
        return reg;
    default:
        printf("%d\n", node->type);
        perror("unkown unary operator");
        exit(1);
    }
}

void gen_while(AstNode *node, RISCV *_asm)
{
    AstWhile *w_stmt = (AstWhile *)node->as;
    Register *reg;
    Label label = create_base_cond_label(LBL_WHILE, _asm);
    Label start_label = extend_label(label, "start");
    Label end_label = extend_label(label, "end");

    // Add label
    emit_label(start_label, _asm);

    reg = eval_asm(w_stmt->expr, _asm);
    // cmp and branch to end label
    fprintf(_asm->out, "\tbeq %s, zero, %s\n", reg->label, end_label);

    free_register(reg);

    _gen_asm(w_stmt->body, _asm);

    // Jump back to start
    emit_jump_label(start_label, _asm);

    // add other label
    emit_label(end_label, _asm);

    free(label);
    free(start_label);
    free(end_label);
}

void gen_for(AstNode *node, RISCV *_asm)
{
    AstFor *f_stmt = (AstFor *)node->as;
    Register *reg = NULL;
    Label label = create_base_cond_label(LBL_FOR, _asm);

    Label init_label = extend_label(label, "init");
    Label cond_label = extend_label(label, "cond");
    Label body_label = extend_label(label, "body");
    Label step_label = extend_label(label, "step");
    Label end_label = extend_label(label, "end");

    emit_label(init_label, _asm);
    // Could be expression or declartion so we need to free register
    if (f_stmt->init->type == AST_VAR_DEC || f_stmt->init->type == AST_VAR_DEF || f_stmt->init->type == AST_EMPTY_EXPR)
    {
        _gen_asm(f_stmt->init, _asm);
    }
    else
    {
        reg = eval_asm(f_stmt->init, _asm);
        free_register(reg);
    }

    emit_label(cond_label, _asm);

    if (f_stmt->cond->type != AST_EMPTY_EXPR)
    {
        reg = eval_asm(f_stmt->cond, _asm);
        // cmp and branch to end label
        fprintf(_asm->out, "\tbeq %s, zero, %s\n", reg->label, end_label);
        free_register(reg);
    }

    emit_label(body_label, _asm);

    _gen_asm(f_stmt->body, _asm);

    emit_label(step_label, _asm);

    if (f_stmt->step->type != AST_EMPTY_EXPR)
    {
        reg = eval_asm(f_stmt->step, _asm);
        free_register(reg);
    }

    emit_jump_label(cond_label, _asm);
    emit_label(end_label, _asm);

    free(init_label);
    free(cond_label);
    free(body_label);
    free(step_label);
    free(end_label);
}

Register *eval_asm(AstNode *node, RISCV *_asm)
{

    switch (node->type)
    {
    case AST_UNARY_EXPR:
        return eval_unary_expr(node, _asm);
    case AST_INT_CONST:
        return eval_int_const(node, _asm);
    case AST_FUNC_CALL:
        return eval_func_call(node, _asm);
    case AST_IDENT:
        return eval_ident(node, _asm);
    case AST_BIN_EXP:
        return eval_bin_exp(node, _asm);
    case AST_VAR_ASGN:
        return eval_var_asgn(node, _asm);
    default:
        printf("%d\n", node->type);
        perror("eval asm unkown ast type");
        exit(1);
    }
}

// Recursively write AST representation to Assembly file
void _gen_asm(AstNode *node, RISCV *_asm)
{
    Register *reg;
    AstExprStmt *expr_stmt;

    switch (node->type)
    {
    case AST_RET:
        gen_ret(node, _asm);
        break;
    case AST_IF:
        gen_if(node, _asm);
        break;
    case AST_COMP_STMT:
        gen_comp_stmt(node, _asm);
        break;
    case AST_FUNC_DEF:
        gen_func_def(node, _asm);
        break;
    case AST_VAR_DEF:
        gen_var_def(node, _asm);
        break;
    case AST_WHILE:
        gen_while(node, _asm);
        break;
    case AST_FOR:
        gen_for(node, _asm);
        break;
    case AST_EXPR_STMT:
        expr_stmt = (AstExprStmt *)node->as;
        if (expr_stmt->expr->type != AST_EMPTY_EXPR)
        {

            reg = eval_asm(expr_stmt->expr, _asm);
            free_register(reg);
        }
        break;
    case AST_ENUM:
    case AST_EMPTY_EXPR:
    case AST_VAR_DEC:
        break;
    default:
        printf("%d\n", node->type);
        perror("gen asm unkown ast type");
        exit(1);
    }
}

void emit_linux_prologue(RISCV *_asm)
{
    fprintf(_asm->out, ".section .text\n");
    fprintf(_asm->out, "\t.globl _start\n");
    fprintf(_asm->out, "_start:\n");
    fprintf(_asm->out, "\tcall main\n");
    fprintf(_asm->out, "\tli a7, 93\n");
    ecall(_asm);
}

void asm_init(RISCV *_asm)
{
    // Create and set up out file
    FILE *out = fopen("./asm.s", "w");
    _asm->out = out;
    emit_linux_prologue(_asm);
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
    free(riscv->zero);
    free(riscv);
}

void ecall(RISCV *_asm)
{
    fprintf(_asm->out, "\tecall\n");
}

// Generate Assembly file from AST
void gen_asm(Vector *prog)
{
    RISCV *_asm = make_riscv();
    asm_init(_asm);
    for (size_t i = 0; i < prog->length; i++)
    {
        _gen_asm((AstNode *)vector_get(prog, i), _asm);
    }

    asm_free(_asm);
}
