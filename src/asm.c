#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "symtab.h"
#include "util.h"
#include "asm.h"

int free_registers[7];
char* registers[7] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};
int sp = 0;

int stack_increase(size_t bytes, FILE* out){
    fprintf(out,"\taddi sp, sp, -%zu\n", bytes);
    sp += bytes;
    return sp;
}


int stack_get(int offset,  FILE* out){
    int reg = alloc_register();
    fprintf(out,"\tlw %s, %d(sp)\n", registers[reg], sp - offset);
    return reg;
}

void stack_store(int offset, int reg, FILE* out){
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


void label_add(char* name, FILE* out){
    fprintf(out,"%s:\n", name);
}

int asm_eval(AstNode* node, SymTab* table, FILE* out){
    int lit;
    int reg;
    int offset;
    int left, right;
    SymTabEntry* var;
    switch(node->type){

        case STATEMENT:

            for(int i =0; i < node->body->length ; i++){
                asm_eval((AstNode*)vector_get(node->body,  i), table,  out);
            }
            return -1;
        
        case LITERAL: 
            str2int(&lit, node->value,10);
            reg = alloc_register();
            load_register(reg,lit,out);
            return reg;

        case FUNC_DEF:
            label_add(node->value, out);
            for(int i =0; i < node->body->length ; i++){
                asm_eval((AstNode*)vector_get(node->body,  i), table,  out);
            }
            return -1;

        case VAR_DEF:
        
            //make space on stack
            offset = stack_increase(sizeof(int),  out);
            //store value on to stack
            reg = asm_eval( node->left,table, out);
            stack_store(offset, reg, out);
            free_register(reg);

            //int reg_to_print = stack_get(offset, out);
            //print_regsiter(reg_to_print, out);
            //pring_newline(out);
            //free_register(reg_to_print);

            //Store location in symbol table
            symtab_get(table, node->value)->offset = offset;
            return -1;
        case VAR:
            //Store location in symbol table
            var = symtab_get(table,  node->value);

            if(var == NULL){
                perror("Cannot use undefined variable.");
                exit(1);
            }
            int re = stack_get(var->offset, out);
            return re; 

        default:
            left = asm_eval(node->left, table, out);
            right = asm_eval(node->right, table, out);
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
    //fprintf(out, "main:\n");
    asm_eval(root, table, out);
    fclose(out);
}
