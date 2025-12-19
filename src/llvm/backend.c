
#include "mylang/hashmap.h"
#include <inttypes.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Types.h>
#include <mylang/ast.h>
#include <mylang/util.h>
#include <stdio.h>
#include <stdlib.h>

LLVMValueRef llvm_eval(AstNode *node, LLVMValueRef llvm_func, LLVMModuleRef mod, LLVMContextRef ctx)
{

    AstIdent *ident;
    AstBinExp *bin_exp;
    AstIntConst *int_const;
    AstFuncCall *func_call;

    LLVMBasicBlockRef bb;
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    switch (node->type)
    {
    case AST_IDENT:
        ident = (AstIdent *)node->as;
        return ident->symbol->llvm_value_ref;
        break;
    case AST_BIN_EXP:
        bin_exp = (AstBinExp *)node->as;
        LLVMOpcode op;
        LLVMValueRef left = llvm_eval(bin_exp->left, llvm_func, mod, ctx);
        LLVMValueRef right = llvm_eval(bin_exp->right, llvm_func, mod, ctx);

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
            break;
        default:
            perror("LLVM op type not implemented");
            exit(1);
        }
        return LLVMBuildBinOp(builder, op, left, right, "binop");
    case AST_INT_CONST:
        int_const = (AstIntConst *)node->as;
        return LLVMConstInt(LLVMInt32Type(), int_const->value, false);
    case AST_FUNC_CALL:
        func_call = (AstFuncCall *)node->as;

        AstNode *n;
        LLVMValueRef *args = NULL;
        size_t arg_length = func_call->args->length;
        if (arg_length > 0)
        {

            args = (LLVMValueRef *)my_malloc(sizeof(LLVMValueRef) * arg_length);

            for (size_t i = 0; i < arg_length; i++)
            {
                n = (AstNode *)vector_get(func_call->args, i);
                args[i] = llvm_eval(n, llvm_func, mod, ctx);
            }
        }

        bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "fcall");
        LLVMPositionBuilderAtEnd(builder, bb);

        return LLVMBuildCall2(builder, func_call->symbol->llvm_type_ref, func_call->symbol->llvm_value_ref, args,
                              arg_length, "");
    default:
        perror("LLVM eval not implemented");
        exit(1);
    }
}
void _llvm_code_gen(AstNode *node, LLVMValueRef llvm_func, LLVMModuleRef mod, LLVMContextRef ctx)
{

    AstCompStmt *comp_stmt;
    AstRet *ret;
    AstVarDec *dec;
    AstVarDef *var_def;
    // AstIfElse *if_stmt;
    // AstEnum *enm;
    // AstVarAsgn *asgn;
    // AstWhile *w_stmt;
    // AstFor *f_stmt;
    // AstExprStmt *expr_stmt;
    // AstUnaryExpr *unary_expr;
    // AstLValue *lvalue;

    LLVMBasicBlockRef bb;
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx);
    switch (node->type)
    {
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;
        for (size_t i = 0; i < comp_stmt->body->length; i++)
            _llvm_code_gen((AstNode *)vector_get(comp_stmt->body, i), llvm_func, mod, ctx);
        break;
    case AST_RET:
        ret = (AstRet *)node->as;

        LLVMValueRef ret_val = llvm_eval(ret->expr, llvm_func, mod, ctx);

        bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "ret");
        LLVMPositionBuilderAtEnd(builder, bb);

        LLVMBuildRet(builder, ret_val);
        break;
    case AST_VAR_DEC:
        dec = (AstVarDec *)node->as;
        bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "var");
        LLVMPositionBuilderAtEnd(builder, bb);

        LLVMValueRef ref = LLVMBuildAlloca(builder, LLVMInt32Type(), dec->value);
        dec->symbol->llvm_value_ref = ref;
        break;
    case AST_VAR_DEF:
        var_def = (AstVarDef *)node->as;

        LLVMValueRef init_val = llvm_eval(var_def->expr, llvm_func, mod, ctx);

        bb = LLVMAppendBasicBlockInContext(ctx, llvm_func, "var def");
        LLVMPositionBuilderAtEnd(builder, bb);

        LLVMValueRef var_def_ref = LLVMBuildAlloca(builder, LLVMInt32Type(), var_def->value);
        LLVMBuildStore(builder, init_val, var_def_ref);

        var_def->symbol->llvm_value_ref = var_def_ref;
        break;
    case AST_IF:
    case AST_BOOL_EXPR:
    case AST_ENUM:
    case AST_VAR_ASGN:
    case AST_WHILE:
    case AST_FOR:
    case AST_EMPTY_EXPR:
    case AST_EXPR_STMT:
    case AST_UNARY_EXPR:
    case AST_LVAL:
    case AST_FUNC_DEF:
    default:
        perror("LLVM not implemented");
        break;
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
    _llvm_code_gen(func->body, llvm_func, mod, ctx);
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
    AstNode *node;
    LLVMContextRef ctx = LLVMContextCreate();
    LLVMModuleRef mod = LLVMModuleCreateWithNameInContext("my-lang", ctx);

    for (size_t i = 0; i < prog->length; i++)
    {
        node = (AstNode *)vector_get(prog, i);
        llvm_func_def((AstFuncDef *)(node->as), mod, ctx);
    }
    printf("Writing llvm mod\n");
    save_module_to_file(mod, "./my-llvm");
    LLVMContextDispose(ctx);
}
