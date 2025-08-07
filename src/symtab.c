#include "symtab.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "util.h"

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define TABLE_SIZE 100

SymTabEntry *make_symtab_entry(char *key, TypeSpecifier type, SymbolType symbol)
{
    SymTabEntry *entry = my_malloc(sizeof(SymTabEntry));
    entry->key = key;
    entry->type = type;
    entry->symbol = symbol;
    entry->is_arg_loaded = false;
    return entry;
}

TableNode *make_node(TableNode *next, SymTabEntry *data)
{
    TableNode *node = my_malloc(sizeof(TableNode));
    node->data = data;
    node->next = next;
    return node;
}

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
// https://benhoyt.com/writings/hash-table-in-c/
static uint64_t hash(const char *key)
{
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++)
    {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Return first entry matching key
// and NULL otherwise
SymTabEntry *find(TableNode *node, char *key)
{
    TableNode *curr = node;
    while (curr != NULL)
    {
        if (!strcmp(curr->data->key, key))
        {
            return curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}

// Insert node at end of list
void insert(TableNode **node, SymTabEntry *entry)
{
    TableNode *n = make_node(NULL, entry);

    // Set node if first item
    if (*node == NULL)
    {
        *node = n;
        return;
    }

    // Find end of list
    TableNode *curr = *node;
    TableNode *prev;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
    }

    // Insert new node at end of list
    prev->next = n;
}

// Add symbol to table, overwrites matching key values
void symtab_add(SymTab *table, SymTabEntry *entry)
{
    int h = hash(entry->key);
    size_t index = (size_t)(h & (uint64_t)(table->size - 1));
    TableNode **node = table->map + index;
    insert(node, entry);
}

// Return entry if it exists else null
SymTabEntry *symtab_get(SymTab *table, char *key)
{
    int h = hash(key);
    size_t index = (size_t)(h & (uint64_t)(table->size - 1));
    TableNode **node = table->map + index;
    return find(*node, key);
}

SymTab *symtab_new()
{
    SymTab *table = my_malloc(sizeof(SymTab));
    TableNode **map = calloc(sizeof(TableNode *), TABLE_SIZE);

    table->map = map;
    table->size = TABLE_SIZE;

    return table;
}

void symtab_free(SymTab *table)
{
    for (int i = 0; i < table->size; i++)
    {
        TableNode *tmp;
        TableNode *curr = table->map[i];

        while (curr)
        {
            SymbolType sym = curr->data->symbol;

            if (sym != SYM_KEYWORD)
                free(curr->data->key);

            // Free  Symbol entry
            free(curr->data);

            tmp = curr->next;

            // Free Table Node
            free(curr);

            curr = tmp;
        }
    }
    free(table->map);
    free(table);
}

void symtab_init(SymTab *table)
{
    symtab_add(table, make_symtab_entry("int", TS_INT, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("void", TS_VOID, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("char", TS_CHAR, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("float", TS_FLOAT, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("double", TS_DOUBLE, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("long", TS_LONG, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("return", TS_VOID, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("if", TS_VOID, SYM_KEYWORD));
    symtab_add(table, make_symtab_entry("enum", TS_VOID, SYM_KEYWORD));
}
