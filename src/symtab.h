#ifndef SYMTAB_H 
#define SYMTAB_H
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct StackFrame {
    //List of variable names as strings (char*)
    Vector* variables; 
    // Size of frame
    size_t size; 
    //Name of function 
    char* func;
} StackFrame;

typedef enum SymbolType{
    FUNCTION,
    VARIABLE,
    KEYWORD,
} SymbolType;

typedef enum Type {
    INT,
    VOID,
    CHAR,
    LONG,
    DOUBLE,
    FLOAT
} Type;

typedef struct SymTabEntry{
    char* key;
    SymbolType symbol;

    //Function return type or Variable type
    Type type;

    //Function args, list of types as strings (char *) 
    Vector* params;

    //Amount of bytes needed for function call
    StackFrame* frame;

    //Variable location, offset from sp in bytes
    size_t offset; 
}SymTabEntry;

typedef struct TableNode{
    struct SymTabEntry* data;
    struct TableNode* next;
}TableNode;

typedef struct SymTab{
    size_t size;
    struct TableNode** map;
} SymTab;

SymTabEntry* make_symtab_entry(char* key,  Type type, SymbolType symbol, Vector* params);
TableNode* make_node(TableNode* next,SymTabEntry* data);
static uint64_t hash(const char* key);
SymTabEntry* find(TableNode* node, char* key);
void insert(TableNode** node, SymTabEntry* entry);
void symtab_add(SymTab* table,SymTabEntry* entry);
SymTabEntry* symtab_get(SymTab* table, char* key);
SymTab* symtab_new();
void symtab_free(SymTab* table);


#endif