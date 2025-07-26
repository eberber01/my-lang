#ifndef SYMTAB_H
#define SYMTAB_H
#include "util.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct StackFrame
{
    // List of variable names as strings (char*)
    // Vector* variables;
    //  Size of frame
    size_t size;
    // Name of function
    char *func;
} StackFrame;

typedef enum SymbolType
{
    SYM_FUNCTION,
    SYM_VARIABLE,
    SYM_KEYWORD,
    SYM_CONST
} SymbolType;

typedef enum TypeSpecifier
{
    TS_INT,
    TS_VOID,
    TS_CHAR,
    TS_LONG,
    TS_DOUBLE,
    TS_FLOAT
} TypeSpecifier;

typedef struct SymTabEntry
{
    char *key;
    SymbolType symbol;

    // Function return type or Variable type
    TypeSpecifier type;

    bool is_arg_loaded;
    int arg_reg;

    // Variable location, offset from sp in bytes
    size_t offset;

    // Function param types (char*)
    Vector *params;

    //Value for enum
    int const_value;
} SymTabEntry;

typedef struct TableNode
{
    struct SymTabEntry *data;
    struct TableNode *next;
} TableNode;

typedef struct SymTab
{
    size_t size;
    struct TableNode **map;
} SymTab;

SymTabEntry *make_symtab_entry(char *key, TypeSpecifier type, SymbolType symbol);
TableNode *make_node(TableNode *next, SymTabEntry *data);
static uint64_t hash(const char *key);
SymTabEntry *find(TableNode *node, char *key);
void insert(TableNode **node, SymTabEntry *entry);
void symtab_add(SymTab *table, SymTabEntry *entry);
SymTabEntry *symtab_get(SymTab *table, char *key);
SymTab *symtab_new();
void symtab_free(SymTab *table);
void symtab_init(SymTab *table);

#endif