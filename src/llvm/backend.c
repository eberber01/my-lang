#include "backend.h"
#include <inttypes.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Types.h>
#include <mylang/ast.h>
#include <mylang/hashmap.h>
#include <mylang/lex.h>
#include <mylang/util.h>
#include <stdio.h>
#include <stdlib.h>

LLVMValueRef llvm_eval_bin_exp(AstBinExp *bin_exp, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    LLVMOpcode op;
    LLVMBasicBlockRef bb;
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMValueRef left = llvm_eval(bin_exp->left, llvm_func, ctx);
    LLVMValueRef right = llvm_eval(bin_exp->right, llvm_func, ctx);

    bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "binop");
    LLVMPositionBuilderAtEnd(builder, bb);

    switch (bin_exp->op_type)
    {
    case TOK_ADD:
        op = LLVMAdd;
        break;
    case TOK_SUB:
        op = LLVMSub;
        break;
    case TOK_MULT:
        op = LLVMMul;
        break;
    case TOK_DIV:
        op = LLVMSDiv;
        break;
    case TOK_EQUAL:
        return LLVMBuildICmp(builder, LLVMIntEQ, left, right, "equal");
    case TOK_NOT_EQUAL:
        return LLVMBuildICmp(builder, LLVMIntNE, left, right, "not equal");
    case TOK_LT:
        return LLVMBuildICmp(builder, LLVMIntULT, left, right, "less than");
    case TOK_LT_EQ:
        return LLVMBuildICmp(builder, LLVMIntULE, left, right, "less than equal");
    case TOK_GT:
        return LLVMBuildICmp(builder, LLVMIntUGT, left, right, "greater than");
    case TOK_GT_EQ:
        return LLVMBuildICmp(builder, LLVMIntUGE, left, right, "greater than equal");
    default:
        perror("LLVM op type not implemented");
        exit(1);
    }
    return LLVMBuildBinOp(builder, op, left, right, "binop");
}

LLVMValueRef llvm_eval_func_call(AstFuncCall *func_call, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    AstNode *n;
    LLVMBasicBlockRef bb;
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMValueRef *args = NULL;
    size_t arg_length = func_call->args->length;
    if (arg_length > 0)
    {

        args = (LLVMValueRef *)my_malloc(sizeof(LLVMValueRef) * arg_length);

        for (size_t i = 0; i < arg_length; i++)
        {
            n = (AstNode *)vector_get(func_call->args, i);
            args[i] = llvm_eval(n, llvm_func, ctx);
        }
    }

    bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "fcall");
    LLVMPositionBuilderAtEnd(builder, bb);

    return LLVMBuildCall2(builder, func_call->symbol->llvm_type_ref, func_call->symbol->llvm_value_ref, args,
                          arg_length, "");
}

LLVMValueRef llvm_eval_var_asgn(AstVarAsgn *var_asgn, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "asgn");
    LLVMValueRef lval = llvm_eval(var_asgn->lval, llvm_func, ctx);
    LLVMValueRef rval = llvm_eval(var_asgn->rval, llvm_func, ctx);
    LLVMPositionBuilderAtEnd(builder, bb);
    return LLVMBuildStore(builder, rval, lval);
}

LLVMValueRef llvm_eval_lval(AstLValue *lvalue)
{
    AstIdent *lval_ident;

    switch (lvalue->kind)
    {
    case AST_LVAL_IDENT:
        lval_ident = (AstIdent *)(lvalue->u.ident->as);
        return lval_ident->symbol->llvm_value_ref;
    default:
        perror("LLVM eval lvalue not implemented");
        exit(1);
    }
}

LLVMValueRef llvm_eval_int_const(AstIntConst *int_const)
{
    return LLVMConstInt(LLVMInt32Type(), int_const->value, false);
}

LLVMValueRef llvm_eval_ident(AstIdent *ident, LLVMValueRef llvm_func, LLVMContextRef ctx)
{
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "ident");
    LLVMPositionBuilderAtEnd(builder, bb);

    return LLVMBuildLoad2(builder, ident->symbol->llvm_type_ref, ident->symbol->llvm_value_ref, "");
}

LLVMValueRef llvm_eval(AstNode *node, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    AstIdent *ident;
    AstBinExp *bin_exp;
    AstIntConst *int_const;
    AstFuncCall *func_call;
    AstVarAsgn *var_asgn;
    AstLValue *lvalue;

    switch (node->type)
    {
    case AST_IDENT:
        ident = (AstIdent *)node->as;
        return llvm_eval_ident(ident, llvm_func, ctx);
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        return llvm_eval_bin_exp(bin_exp, llvm_func, ctx);
    case AST_INT_CONST:
        int_const = (AstIntConst *)node->as;
        return llvm_eval_int_const(int_const);
    case AST_FUNC_CALL:
        func_call = (AstFuncCall *)node->as;
        return llvm_eval_func_call(func_call, llvm_func, ctx);
    case AST_LVAL:
        lvalue = (AstLValue *)node->as;
        return llvm_eval_lval(lvalue);
    case AST_VAR_ASGN:
        var_asgn = (AstVarAsgn *)node->as;
        return llvm_eval_var_asgn(var_asgn, llvm_func, ctx);
    default:
        perror("LLVM eval not implemented");
        exit(1);
    }
}

LLVMBasicBlockRef llvm_gen_if(AstIfElse *if_stmt, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMBasicBlockRef bb;
    LLVMBasicBlockRef cond_block = LLVMCreateBasicBlockInContext(ctx, "cond");
    LLVMBasicBlockRef then_block = LLVMCreateBasicBlockInContext(ctx, "then");
    LLVMBasicBlockRef else_block = LLVMCreateBasicBlockInContext(ctx, "else");
    LLVMBasicBlockRef cont_block = LLVMCreateBasicBlockInContext(ctx, "ifcont");

    // If
    LLVMValueRef cond = llvm_eval(if_stmt->if_expr, llvm_func, ctx);

    LLVMAppendExistingBasicBlock(llvm_func, cond_block);
    LLVMPositionBuilderAtEnd(builder, cond_block);
    LLVMBuildCondBr(builder, cond, then_block, else_block);

    // then
    LLVMPositionBuilderAtEnd(builder, then_block);
    LLVMAppendExistingBasicBlock(llvm_func, then_block);
    bb = _llvm_code_gen(if_stmt->if_body, llvm_func, ctx);
    LLVMPositionBuilderAtEnd(builder, bb);
    LLVMBuildBr(builder, cont_block);

    // Else
    if (if_stmt->else_body != NULL)
    {
        LLVMPositionBuilderAtEnd(builder, else_block);
        LLVMAppendExistingBasicBlock(llvm_func, else_block);
        bb = _llvm_code_gen(if_stmt->else_body, llvm_func, ctx);
        LLVMPositionBuilderAtEnd(builder, bb);
        LLVMBuildBr(builder, cont_block);
    }

    LLVMAppendExistingBasicBlock(llvm_func, cont_block);
    return cont_block;
}

LLVMBasicBlockRef llvm_gen_ret(AstRet *ret, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMValueRef ret_val = llvm_eval(ret->expr, llvm_func, ctx);

    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "ret");
    LLVMPositionBuilderAtEnd(builder, bb);

    LLVMBuildRet(builder, ret_val);
    return bb;
}

LLVMBasicBlockRef llvm_gen_var_dec(AstVarDec *dec, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "var");
    LLVMPositionBuilderAtEnd(builder, bb);

    LLVMTypeRef type_ref = LLVMInt32Type();
    LLVMValueRef ref = LLVMBuildAlloca(builder, type_ref, dec->value);
    dec->symbol->llvm_value_ref = ref;
    dec->symbol->llvm_type_ref = type_ref;
    return bb;
}

LLVMBasicBlockRef llvm_gen_var_def(AstVarDef *var_def, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    LLVMValueRef init_val = llvm_eval(var_def->expr, llvm_func, ctx);

    LLVMBasicBlockRef bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "var def");
    LLVMPositionBuilderAtEnd(builder, bb);

    LLVMTypeRef var_type_ref = LLVMInt32Type();
    LLVMValueRef var_def_ref = LLVMBuildAlloca(builder, var_type_ref, var_def->value);
    LLVMBuildStore(builder, init_val, var_def_ref);

    var_def->symbol->llvm_value_ref = var_def_ref;
    var_def->symbol->llvm_type_ref = var_type_ref;
    return bb;
}

LLVMBasicBlockRef _llvm_code_gen(AstNode *node, LLVMValueRef llvm_func, LLVMContextRef ctx)
{

    AstCompStmt *comp_stmt;
    AstRet *ret;
    AstVarDec *dec;
    AstVarDef *var_def;
    AstIfElse *if_stmt;
    AstExprStmt *expr_stmt;
    // AstEnum *enm;
    // AstVarAsgn *asgn;
    // AstWhile *w_stmt;
    // AstFor *f_stmt;
    // AstUnaryExpr *unary_expr;
    LLVMBasicBlockRef bb;

    switch (node->type)
    {
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;

        for (size_t i = 0; i < comp_stmt->body->length; i++)
            bb = _llvm_code_gen((AstNode *)vector_get(comp_stmt->body, i), llvm_func, ctx);
        return bb;
    case AST_RET:
        ret = (AstRet *)node->as;
        return llvm_gen_ret(ret, llvm_func, ctx);
    case AST_VAR_DEC:
        dec = (AstVarDec *)node->as;
        return llvm_gen_var_dec(dec, llvm_func, ctx);
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;
        return llvm_gen_var_def(var_def, llvm_func, ctx);
    case AST_EXPR_STMT:
        expr_stmt = (AstExprStmt *)node->as;
        if (expr_stmt->expr->type != AST_EMPTY_EXPR)
            llvm_eval(expr_stmt->expr, llvm_func, ctx);
        return LLVMAppendBasicBlockInContext(ctx, llvm_func, "");
    case AST_IF:
        if_stmt = (AstIfElse *)node->as;
        return llvm_gen_if(if_stmt, llvm_func, ctx);
    case AST_EMPTY_EXPR:
        return LLVMAppendBasicBlockInContext(ctx, llvm_func, "");
    case AST_ENUM:
    case AST_WHILE:
    case AST_FOR:
    case AST_UNARY_EXPR:
    case AST_FUNC_DEF:
    default:
        fprintf(stderr, "LLVM not implemented type: %d\n", node->type);
        exit(1);
    }
}

void llvm_func_def(AstFuncDef *func, LLVMModuleRef mod, LLVMContextRef ctx)
{
    LLVMTypeRef ret_type;
    size_t param_count = func->params->length;
    LLVMTypeRef *param_types = (LLVMTypeRef *)my_malloc(sizeof(LLVMTypeRef) * param_count);

    for (size_t i = 0; i < param_count; i++)
    {
        // Change this later for different types
        param_types[i] = LLVMInt32Type();
    }

    if (!strcmp("int", func->type))
        ret_type = LLVMInt32Type();
    else
        ret_type = LLVMVoidType();

    LLVMTypeRef func_type = LLVMFunctionType(ret_type, param_types, param_count, false);
    LLVMValueRef llvm_func = LLVMAddFunction(mod, func->value, func_type);
    func->symbol->llvm_value_ref = llvm_func;
    func->symbol->llvm_type_ref = func_type;
    _llvm_code_gen(func->body, llvm_func, ctx);
}

void save_module_to_file(LLVMModuleRef module, const char *filename)
{
    char *errorMessage = NULL;
    if (LLVMPrintModuleToFile(module, filename, &errorMessage) != 0)
    {
        fprintf(stderr, "Error writing LLVM IR to file: %s\n", errorMessage);
        LLVMDisposeMessage(errorMessage); // Free the error
    }
    else
    {
        printf("Successfully wrote LLVM IR to %s\n", filename);
    }
}

void llvm_code_gen(Vector *prog)
{
    const char *triple = "riscv32-unknown-linux-gnu";
    const char *cpu = "";
    const char *features = "+m,+a,+f,+d,+c";

    LLVMInitializeRISCVTargetInfo();
    LLVMInitializeRISCVTarget();
    LLVMInitializeRISCVTargetMC();
    LLVMInitializeRISCVAsmPrinter();

    LLVMContextRef ctx = LLVMContextCreate();
    LLVMModuleRef mod = LLVMModuleCreateWithNameInContext("my-lang", ctx);

    LLVMTargetRef target;
    char *error = NULL;

    if (LLVMGetTargetFromTriple(triple, &target, &error))
    {
        fprintf(stderr, "LLVMGetTargetFromTriple error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(1);
    }

    LLVMTargetMachineRef tm = LLVMCreateTargetMachine(target, triple, cpu, features, LLVMCodeGenLevelDefault,
                                                      LLVMRelocDefault, LLVMCodeModelDefault);
    if (!tm)
    {
        fprintf(stderr, "LLVMCreateTargetMachine failed\n");
        exit(1);
    }

    LLVMTargetDataRef dl = LLVMCreateTargetDataLayout(tm);
    LLVMSetModuleDataLayout(mod, dl);
    LLVMSetTarget(mod, triple);

    for (size_t i = 0; i < prog->length; i++)
    {
        AstNode *node = (AstNode *)vector_get(prog, i);
        llvm_func_def((AstFuncDef *)(node->as), mod, ctx);
    }

    save_module_to_file(mod, "./my-llvm.ll");

    /*
    char *err2 = NULL;
    if (LLVMTargetMachineEmitToFile(tm, mod, "out.o", LLVMObjectFile, &err2)) {
        fprintf(stderr, "Emit error: %s\n", err2);
        LLVMDisposeMessage(err2);
        exit(1);
    }
    */

    LLVMDisposeModule(mod);
    LLVMDisposeTargetData(dl);
    LLVMDisposeTargetMachine(tm);
    LLVMContextDispose(ctx);
}
