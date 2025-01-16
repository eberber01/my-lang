#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"
#include "util.h"
#include "asm.h"

int free_registers[7];
char* registers[8] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6", "ra"};
int sp = 0;

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

void sp_increase(size_t bytes, FILE* out){
    fprintf(out,"\taddi sp, sp, -%zu\n", bytes);
    sp += bytes;
}
void sp_decrease(size_t bytes, FILE* out){
    fprintf(out,"\taddi sp, sp, %zu\n", bytes);
    sp -= bytes;
}

//Load value at sp offset into reg
int sp_load(int reg, int offset,  FILE* out){
    fprintf(out,"\tlw %s, %d(sp)\n", registers[reg], sp - offset);
    return reg;
}

//Store value at reg at sp plus offset
void sp_store(int offset, int reg, FILE* out){
    fprintf(out,"\tsw %s, %d(sp)\n", registers[reg], sp - offset);
}

void pring_newline(FILE * out){
    fprintf(out, "\tli a7, 11\n" );          // service 11 is print integer
    int r = alloc_register();
    load_register(r, 10, out);
    fprintf(out, "\tadd a0, %s, zero\n", registers[r]);  // load desired value into argument register a0, using pseudo-op
    fprintf(out, "\tecall\n");
    free_register(r);

}

void print_regsiter(int reg, FILE* out){
    fprintf(out, "\tli  a7, 1\n" );          // service 1 is print integer
    fprintf(out, "\tadd a0, %s, zero\n", registers[reg]);  // load desired value into argument register a0, using pseudo-op
    fprintf(out, "\tecall\n");
}

int alloc_register(){
    for(int i=0; i < 7; i++){
        if(free_registers[i] != 1){
            free_registers[i] = 1;
            return i;
        }
    }
    perror("No free registers");
    exit(1);
}

void free_register(int reg){
    free_registers[reg] = 0;
}
int reg_ret(FILE* out){
    fprintf(out,"\tret\n");
}
int reg_add(int a, int b, FILE* out){
    fprintf(out,"\tadd %s, %s, %s\n", registers[a],registers[a], registers[b]);
    free_register(b);
    return a;
}
int reg_sub(int a, int b, FILE* out){
    fprintf(out,"\tsub %s, %s, %s\n", registers[a], registers[a], registers[b]);
    free_register(b);
    return a;

}
int reg_mult(int a, int b, FILE* out){
    fprintf(out,"\tmul %s, %s, %s\n", registers[a], registers[a], registers[b]);
    free_register(b);
    return a;
}
int reg_div(int a, int b, FILE* out){
    fprintf(out,"\tdiv %s, %s, %s\n", registers[a], registers[a], registers[b]);
    free_register(b);
    return a;
}

int load_register(int reg, int value, FILE* out){
    fprintf(out,"\taddi %s, zero, %d\n", registers[reg], value);
    return reg;
}

void jump_to_label(char* label, FILE* out){
    fprintf(out, "\tjal %s\n", label);
}
void return_from_jump(FILE* out){
    fprintf(out, "\tjalr ra\n");
}

void label_add(char* name, FILE* out){
    fprintf(out,"%s:\n", name);
}


int asm_eval(AstNode* node, SymTab* table, StackFrame* frame,FILE* out){
    int lit;
    int reg;
    int offset;
    int left, right;
    SymTabEntry* var;
    switch(node->type){
        case AST_FUNC_CALL:
            //Allocate space for return address 
            sp_increase(sizeof(void*), out);
            sp_store(0,  7,  out);


            //Save registers
            for(int i =0; i < 7; i++){
                if(free_registers[i] == 1){
                    // 
                    sp_increase(sizeof(void*), out);
                    sp_store(0, i, out);
                }
            }


            //Get frame and allocate enough space for call
            size_t bytes = symtab_get(table,  node->value)->frame->size;
            sp_increase(bytes, out);


            //TODO: parameter passing
            jump_to_label(node->value, out);
            
            //Restore stack
            sp_decrease(bytes, out);

            //Restore registers
            for(int i =0; i < 7; i++){
                if(free_registers[i] == 1){
                    sp_decrease(sizeof(void*), out);
                    sp_load(i, 0, out);
                }
            }

            //restore return address
            sp_decrease(sizeof(void*), out);
            sp_load(7,  0,  out);

            //TODO load register with return value
            reg = alloc_register();
            fprintf(out, "\tadd %s, zero, a0\n", registers[reg]);
            return reg;
        case AST_IF:
            // Add label
            label_add("if_start", out);

            //eval bool expression, reg = 0 for false and reg = 1 for true
           reg = asm_eval(node->left, table, frame, out);

            //cmp and branch to end label
            fprintf(out, "\tbeq %s, zero, if_end\n", registers[reg]);
            label_add("if_body", out);
            //eval body
            for(int i =0; i < node->body->length ; i++){
                 asm_eval((AstNode*)vector_get(node->body,  i), table,frame,  out);
            }

            //add other label
            label_add("if_end", out);
            free_register(reg);
            return -1; 


        case AST_RET:

            //return val
            reg = asm_eval(node->left, table,frame, out);

            //Exit status 0, if main function 
            if(!strcmp(frame->func, "main")){
                //Syscall Exit 10
                fprintf(out, "\tli  a7, 10\n" );   
                fprintf(out, "\tecall\n");
            }
            else{
                // move to return a0
                fprintf(out, "\tadd a0, %s, zero\n", registers[reg]);
            } 
            free_register(reg);
            return -1;

        case AST_STATEMENT:

            for(int i =0; i < node->body->length ; i++){
                asm_eval((AstNode*)vector_get(node->body,  i), table,frame,  out);
            }
            return -1;
        
        case AST_LITERAL: 
            str2int(&lit, node->value,10);
            reg = alloc_register();
            load_register(reg,lit,out);
            return reg;

        case AST_FUNC_DEF:
            label_add(node->value, out);
            //Create New StackFrame
            StackFrame* f = make_stack_frame(node->value);
            for(int i =0; i < node->body->length ; i++){
                asm_eval((AstNode*)vector_get(node->body,  i), table, f, out);
            }
            //Store function frame
            symtab_get(table,node->value)->frame = f;
            
            if(strcmp(node->value, "main")){
                //return 
                fprintf(out,"\tjalr ra\n");
            }
            return -1;

        case AST_VAR_DEF:
            //Increase stack frame size
            offset = stackframe_add(frame, node->value,  symtab_get(table,  node->value)->type);

            //store value on to stack
            reg = asm_eval( node->left,table, frame, out);
            sp_store(offset, reg, out);
            free_register(reg);

            //Store location in symbol table
            symtab_get(table, node->value)->offset = offset;
            return -1;
        case AST_VAR:
            //Store location in symbol table
            var = symtab_get(table,  node->value);

            if(var == NULL){
                perror("Cannot use undefined variable.");
                exit(1);
            }
            //Load value from sp
            reg = alloc_register();
            sp_load(reg, var->offset, out);
            return reg;

        case AST_BOOL_EXPR:
            //  evaulte expr
            left = asm_eval(node->left, table, frame,out);
            right = asm_eval(node->right, table, frame, out);

            reg = alloc_register();
            load_register(reg, 0, out);
            // == Check if equal 
            fprintf(out, "\tbne %s, %s, bool_end\n", registers[left], registers[right]);

            //load register with One if true
            load_register(reg, 1, out);

            label_add("bool_end", out);

            return reg;
        default:
            left = asm_eval(node->left, table, frame,out);
            right = asm_eval(node->right, table, frame, out);
            switch (*(node->value)) {
                case '+':
                    return reg_add(left, right, out);
                case '-':
                    return reg_sub(left, right, out);
                case '*':
                    return reg_mult(left, right, out);
                case '/':
                    return reg_div(left, right, out);
                default:
                    perror("Unexpected node type.");
                    exit(1);
            }

    }

}

void gen_asm(AstNode* root, SymTab* table){
    FILE* out = fopen("../asm", "w");
    fprintf(out, ".globl main\n\n");
    fprintf(out, "\tj main\n");

    asm_eval(root, table, NULL, out);
    fclose(out);
}
