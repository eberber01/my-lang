#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "util.h"
#include "asm.h"

Register* make_register(char* label){
    Register* reg = my_malloc(sizeof(Register));
    reg->label = label;
    reg->free = 1;
    return reg;
}

RISCV* make_riscv(){
    RISCV* riscv = my_malloc(sizeof(RISCV));
    Vector* temp_reg = vector_new();
    Vector* arg_reg = vector_new();

    //Temp registers
    char* temp[7] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};
    char* arg[8] = {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};
    for(int i=0 ; i < 7; i++){
        vector_push(temp_reg, make_register(temp[i]));
    }
    for(int i=0 ; i < 8; i++){
        vector_push(arg_reg, make_register(arg[i]));
    }
    riscv->temp = temp_reg;
    riscv->arg = arg_reg;
    riscv->ret = make_register("ra");
    riscv->sp = make_register("sp");
    riscv->sp->value = 0;
    return  riscv;
}

StackFrame* make_stack_frame(char* func){
    StackFrame* frame = my_malloc(sizeof(StackFrame));
    Vector* variables = vector_new();

    frame->size = 0;
    frame->variables = variables;
    frame->func = func; 
    return frame;
}
//Returns the offset of the 
int stackframe_add(StackFrame* frame, char* var_name, Type var_type){
    vector_push(frame->variables, var_name);

    //Change for different Types
    int tmp = frame->size;
    frame->size += 4;


    return tmp; 
}

void sp_increase(size_t bytes, RISCV* _asm){
    fprintf(_asm->out,"\taddi sp, sp, -%zu\n", bytes);
    _asm->sp->value += bytes;
}
void sp_decrease(size_t bytes, RISCV* _asm){
    fprintf(_asm->out,"\taddi sp, sp, %zu\n", bytes);
    _asm->sp->value -= bytes;
}

//Load value at sp offset into reg
Register* sp_load(Register* reg, int offset,  RISCV* _asm){
    fprintf(_asm->out,"\tlw %s, %d(sp)\n", reg->label, (_asm->sp->value) - offset);
    return reg;
}

//Store value at reg at sp plus offset
void sp_store(int offset, Register* reg, RISCV* _asm){
    fprintf(_asm->out,"\tsw %s, %d(sp)\n", reg->label, (_asm->sp->value) - offset);
}

void print_newline(RISCV * _asm){
    fprintf(_asm->out, "\tli a7, 11\n" );          // service 11 is print integer
    Register* r = alloc_register(_asm);
    load_register(r, 10, _asm);
    fprintf(_asm->out, "\tadd a0, %s, zero\n", r->label);  // load desired value into argument register a0, using pseudo-op
    fprintf(_asm->out, "\tecall\n");
    free_register(r);

}

void print_register(Register* reg, RISCV* _asm){
    fprintf(_asm->out, "\tli  a7, 1\n" );          // service 1 is print integer
    fprintf(_asm->out, "\tadd a0, %s, zero\n", reg->label);  // load desired value into argument register a0, using pseudo-op
    fprintf(_asm->out, "\tecall\n");
}

// Allocate free temporary register
Register* alloc_register(RISCV* _asm){
    for(int i = 0; i < _asm->temp->length; i++){
        Register* reg = vector_get(_asm->temp,  i);
        if(reg->free){
            reg->free = 0;
            return reg;
        }
    }
    perror("No free registers");
    exit(1);
}

void free_register(Register* reg){
    reg->free = 1;
}

// Register Addition
// Free's Reg2
Register* reg_add(Register* reg1, Register* reg2 , RISCV* _asm){
    fprintf(_asm->out,"\tadd %s, %s, %s\n", reg1->label,reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

// Register Subtraction
// Free's Reg2
Register* reg_sub(Register* reg1, Register* reg2, RISCV* _asm){
    fprintf(_asm->out,"\tsub %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

//Register Multiplication
// Free's Reg2
Register* reg_mult(Register* reg1, Register* reg2, RISCV* _asm){
   fprintf(_asm->out,"\tmul %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

//Register Division 
// Free's Reg2
Register* reg_div(Register* reg1, Register* reg2, RISCV* _asm){
    fprintf(_asm->out,"\tdiv %s, %s, %s\n", reg1->label, reg1->label, reg2->label);
    free_register(reg2);
    return reg1;
}

//Load register with int value
Register* load_register(Register* reg, int value, RISCV* _asm){
    fprintf(_asm->out,"\taddi %s, zero, %d\n", reg->label, value);
    return reg;
}

void jump_to_label(char* label, RISCV* _asm){
    fprintf(_asm->out, "\tjal %s\n", label);
}

void return_from_jump(RISCV* _asm){
    fprintf(_asm->out, "\tjalr ra\n");
}

// Add Label to RISCV out file
// name:
void label_add(char* name, RISCV* _asm){
    fprintf(_asm->out,"%s:\n", name);
}

//Recursively write AST representation to Assembly file
Register* asm_eval(AstNode* node, SymTab* table, StackFrame* frame,RISCV* _asm){
    int lit;
    int offset;
    Register* reg; 
    Register* left; 
    Register* right;
    SymTabEntry* var;
    switch(node->type){
        case AST_FUNC_CALL:
            //Allocate space for return address 
            sp_increase(sizeof(void*), _asm);
            //Store return register
            sp_store(0,  _asm->ret,  _asm);


            //Save registers
            for(int i =0; i < _asm->temp->length; i++){
                reg = vector_get(_asm->temp, i)  ;
                if(!reg->free){
                    sp_increase(sizeof(void*), _asm);
                    sp_store(sizeof(void*), reg, _asm);
                }
            }

            size_t bytes = symtab_get(table,  node->value)->frame->size;
            if(bytes > 0 ){
                sp_increase(bytes, _asm);
            }

            //TODO: parameter passing
            jump_to_label(node->value, _asm);
            
            //Restore stack
            if(bytes > 0 ){
                sp_decrease(bytes, _asm);
            }

            //Restore registers
            for(int i =0; i < _asm->temp->length; i++){
                reg = vector_get(_asm->temp, i)  ;
                if(!reg->free ){
                    sp_decrease(sizeof(void*), _asm);
                    sp_load(reg, sizeof(void*), _asm);
                }
            }

            //restore return address
            sp_decrease(sizeof(void*), _asm);
            sp_load(_asm->ret,  0,  _asm);

            //TODO load register with return value
            reg = alloc_register(_asm);
            fprintf(_asm->out, "\tadd %s, zero, a0\n", reg->label);
            return reg;
        case AST_IF:
            // Add label
            label_add("if_start", _asm);

            //eval bool expression, reg = 0 for false and reg = 1 for true
           reg = asm_eval(node->left, table, frame, _asm);

            //cmp and branch to end label
            fprintf(_asm->out, "\tbeq %s, zero, if_end\n", reg->label);
            label_add("if_body", _asm);
            //eval body
            for(int i =0; i < node->body->length ; i++){
                 asm_eval((AstNode*)vector_get(node->body,  i), table,frame,  _asm);
            }

            //add other label
            label_add("if_end", _asm);
            free_register(reg);
            return NULL; 


        case AST_RET:

            //return val
            reg = asm_eval(node->left, table,frame, _asm);

            //Exit status 0, if main function 
            if(!strcmp(frame->func, "main")){
                //Syscall Exit 10
                fprintf(_asm->out, "\tli  a7, 10\n" );   
                fprintf(_asm->out, "\tecall\n");
            }
            else{
                // move to return a0
                fprintf(_asm->out, "\tadd a0, %s, zero\n", reg->label);
            } 
            free_register(reg);
            return NULL;

        case AST_STATEMENT:

            for(int i =0; i < node->body->length ; i++){
                asm_eval((AstNode*)vector_get(node->body,  i), table,frame,  _asm);
            }
            return NULL;
        
        case AST_LITERAL: 
            str2int(&lit, node->value,10);
            reg = alloc_register(_asm);
            load_register(reg,lit,_asm);
            return reg;

        case AST_FUNC_DEF:
            label_add(node->value, _asm);
            //Create New StackFrame
            StackFrame* f = make_stack_frame(node->value);
            for(int i =0; i < node->body->length ; i++){
                asm_eval((AstNode*)vector_get(node->body,  i), table, f, _asm);
            }
            //Store function frame
            symtab_get(table,node->value)->frame = f;

            //Not main function, return
            if(strcmp(node->value, "main")){
                return_from_jump(_asm);
            }

            return NULL;

        case AST_VAR_DEF:
            //Increase stack frame size
            offset = stackframe_add(frame, node->value,  symtab_get(table,  node->value)->type);

            //store value on to stack
            reg = asm_eval( node->left,table, frame, _asm);
            sp_store(offset, reg, _asm);
            free_register(reg);

            //Store location in symbol table
            symtab_get(table, node->value)->offset = offset;
            return NULL; 
        case AST_VAR:
            //Store location in symbol table
            var = symtab_get(table,  node->value);

            if(var == NULL){
                perror("Cannot use undefined variable.");
                exit(1);
            }
            //Load value from sp
            reg = alloc_register(_asm);
            sp_load(reg, var->offset, _asm);
            return reg;

        case AST_BOOL_EXPR:
            //  evaulte expr
            left = asm_eval(node->left, table, frame,_asm);
            right = asm_eval(node->right, table, frame, _asm);

            reg = alloc_register(_asm);
            load_register(reg, 0, _asm);
            // == Check if equal 
            fprintf(_asm->out, "\tbne %s, %s, bool_end\n", left->label, right->label);

            //load register with One if true
            load_register(reg, 1, _asm);

            label_add("bool_end", _asm);

            return reg;
        default:
            left = asm_eval(node->left, table, frame,_asm);
            right = asm_eval(node->right, table, frame, _asm);
            switch (*(node->value)) {
                case '+':
                    return reg_add(left, right, _asm);
                case '-':
                    return reg_sub(left, right, _asm);
                case '*':
                    return reg_mult(left, right, _asm);
                case '/':
                    return reg_div(left, right, _asm);
                default:
                    perror("Unexpected node type.");
                    exit(1);
            }

    }

}

void asm_init(RISCV* riscv){
    // Create and set up out file
    FILE* out = fopen("../asm", "w");
    fprintf(out, ".globl main\n\n");
    fprintf(out, "\tj main\n");
    riscv->out = out;
}

void asm_free(RISCV* riscv){
    fclose(riscv->out);
    vector_free(riscv->arg);
    vector_free(riscv->temp);
    free(riscv->sp);
    free(riscv->ret);
    free(riscv);
}

//Generate Assembly file from AST
void gen_asm(AstNode* root, SymTab* table){
    RISCV* _asm = make_riscv();
    asm_init(_asm);
    asm_eval(root, table, NULL, _asm);
    asm_free(_asm);
}
