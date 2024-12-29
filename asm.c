#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include "util.h"
#include "asm.h"

int free_registers[7];
char* registers[7] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};

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

int asm_eval(struct AstNode* node, FILE* out){
    if(node->type == LITERAL){
        int lit;
        str2int(&lit, node->value,10);
        int reg = alloc_register();
        load_register(reg,lit,out);
        return reg;
    }


    int left = asm_eval(node->left, out);
    int right = asm_eval(node->right, out);
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

void gen_asm(struct AstNode* root){
    FILE* out = fopen("asm", "w");
    fprintf(out, ".globl main\n\n");
    fprintf(out, "main:\n");
    asm_eval(root, out);
    fclose(out);
}
