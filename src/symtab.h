#ifndef SYMTAB_H 
#define SYMTAB_H
#include "lex.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct SymTabEntry{
    enum TokenType type;
    char* value;
    char* key;
}SymTabEntry;

typedef struct TableNode{
    struct SymTabEntry* data;
    struct TableNode* next;
}TableNode;

typedef struct SymTab{
    size_t size;
    struct TableNode** map;
} SymTab;

SymTabEntry* make_symtab_entry(char* key, char* value, TokenType type);
TableNode* make_node(TableNode* next,SymTabEntry* data);
static uint64_t hash(const char* key);
SymTabEntry* find(TableNode* node, char* key);
void insert(TableNode** node, SymTabEntry* entry);
void symtab_add(SymTab* table,SymTabEntry* entry);
SymTabEntry* symtab_get(SymTab* table, char* key);
SymTab* symtab_new();
void symtab_free(SymTab* table);


#endif