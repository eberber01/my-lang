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

typedef struct TypeEnvEntry
{
    char *value;
    TypeSpecifier ts;
} TypeEnvEntry;

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

    // Value for enum
    int const_value;

    int scope_id;

    StackFrame *frame;
} SymTabEntry;

typedef struct TableNode
{
    void *data;
    char *key;
    struct TableNode *next;

} TableNode;

typedef struct HashMap
{
    size_t size;
    struct TableNode **table;
} HashMap;

SymTabEntry *make_symtab_entry(char *key, TypeSpecifier type, SymbolType symbol);

TableNode *make_table_node(TableNode *next, void *data, char *key);

void *find(TableNode *node, char *key);

void insert(TableNode **node, void *entry, char *key);

void hashmap_add(HashMap *map, void *entry, char *key);

void *hashmap_get(HashMap *map, char *key);

HashMap *hashmap_new(void);

void hashmap_free(HashMap *table);

void type_env_free(HashMap *table);

void type_env_init(HashMap *table);

HashMap *symtab_clone(HashMap *table);

#endif
