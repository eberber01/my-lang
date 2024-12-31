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

struct SymTabEntry* make_symtab_entry(char* key, char* value, TokenType type);
struct TableNode* make_node(struct TableNode* next, struct SymTabEntry* data);
static uint64_t hash(const char* key);
struct SymTabEntry* find(struct TableNode* node, char* key);
void insert(TableNode** node, struct SymTabEntry* entry);
void symtab_add(struct SymTab* table,struct SymTabEntry* entry);
struct SymTabEntry* symtab_get(struct SymTab* table, char* key);
struct SymTab* symtab_new();


#endif