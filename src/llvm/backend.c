
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

LLVMValueRef _llvm_code_gen(AstNode *node, LLVMValueRef llvm_func, LLVMModuleRef mod)
{

    AstCompStmt *comp_stmt;
    AstIdent *ident;
    AstRet *ret;
    AstVarDec *dec;
    // AstIfElse *if_stmt;
    // AstVarDef *var_def;
    // AstBinExp *bin_exp;
    // AstFuncDef *func_def;
    // AstFuncCall *func_call;
    // AstEnum *enm;
    // AstVarAsgn *asgn;
    // AstWhile *w_stmt;
    // AstFor *f_stmt;
    // AstExprStmt *expr_stmt;
    // AstUnaryExpr *unary_expr;
    // AstLValue *lvalue;

    LLVMBasicBlockRef bb;
    LLVMBuilderRef builder;
    switch (node->type)
    {
    case AST_COMP_STMT:
        comp_stmt = (AstCompStmt *)node->as;
        for (size_t i = 0; i < comp_stmt->body->length; i++)
        {
            _llvm_code_gen((AstNode *)vector_get(comp_stmt->body, i), llvm_func, mod);
        }
        break;
    case AST_RET:
        ret = (AstRet *)node->as;
        bb = LLVMAppendBasicBlock(llvm_func, "ret");

        builder = LLVMCreateBuilder();
        LLVMPositionBuilderAtEnd(builder, bb);

        LLVMValueRef ret_val = _llvm_code_gen(ret->expr, llvm_func, mod);

        LLVMBuildRet(builder, ret_val);
        break;
    case AST_VAR_DEC:
        dec = (AstVarDec *)node->as;
        bb = LLVMAppendBasicBlock(llvm_func, "var");

        builder = LLVMCreateBuilder();
        LLVMPositionBuilderAtEnd(builder, bb);

        LLVMValueRef ref = LLVMBuildAlloca(builder, LLVMInt32Type(), dec->value);
        dec->symbol->llvm_ref = ref;
        break;
    case AST_IDENT:
        ident = (AstIdent *)node->as;
        return ident->symbol->llvm_ref;
    case AST_FUNC_CALL:
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
    case AST_INT_CONST:
    case AST_BIN_EXP:
    case AST_VAR_DEF:
        perror("LLVM not implemented");
        break;
    }
    return NULL;
}

void gen_func_def(AstFuncDef *func, LLVMModuleRef mod)
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

    _llvm_code_gen(func->body, llvm_func, mod);
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
    LLVMModuleRef mod = LLVMModuleCreateWithName("my-lang");

    for (size_t i = 0; i < prog->length; i++)
    {
        node = (AstNode *)vector_get(prog, i);
        gen_func_def((AstFuncDef *)(node->as), mod);
    }
    printf("Writing llvm mod\n");
    save_module_to_file(mod, "./my-llvm");
}
