#ifndef LLVM_BACKEND_H
#define LLVM_BACKEND_H

#include <llvm-c/Types.h>
#include <mylang/ast.h>
#include <mylang/util.h>
void llvm_code_gen(Vector *prog);

LLVMBasicBlockRef _llvm_code_gen(AstNode *node, LLVMValueRef llvm_func, LLVMContextRef ctx);

LLVMValueRef llvm_eval(AstNode *node, LLVMValueRef llvm_func, LLVMContextRef ctx);
#endif
