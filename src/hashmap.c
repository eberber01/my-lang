#include <mylang/arena.h>
#include <mylang/hashmap.h>
#include <mylang/util.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define TABLE_SIZE 100

TypeEnvEntry *make_type_env_entry(char *value, TypeSpecifier ts)
{
    TypeEnvEntry *entry = context_alloc(sizeof(TypeEnvEntry));
    entry->value = value;
    entry->ts = ts;
    return entry;
}

SymTabEntry *make_symtab_entry(char *key, TypeSpecifier type, SymbolType symbol)
{
    SymTabEntry *entry = context_alloc(sizeof(SymTabEntry));
    entry->key = key;
    entry->type = type;
    entry->symbol = symbol;
    entry->is_arg_loaded = false;
    return entry;
}

TableNode *make_table_node(TableNode *next, void *data, char *key)
{
    TableNode *node = my_malloc(sizeof(TableNode));
    node->data = data;
    node->next = next;
    node->key = key;
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

// Return matching entry for given key
// or NULL otherwise if it does not exist
void *find(TableNode *node, char *key)
{
    TableNode *curr = node;
    while (curr != NULL)
    {
        if (!strcmp(curr->key, key))
        {
            return curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}

// Insert node at end of list
void insert(TableNode **index_node, void *data, char *key)
{

    TableNode *n = make_table_node(NULL, data, key);

    // Set node if first item
    if (*index_node == NULL)
    {
        *index_node = n;
        return;
    }

    // Overwrite data if duplicate key exists
    // Otherwise add new node to end of list
    TableNode *curr = *index_node;
    TableNode *prev;
    while (curr != NULL)
    {
        if (!strcmp(key, curr->key))
        {
            curr->data = data;
            free(n);
            return;
        }

        prev = curr;
        curr = curr->next;
    }

    // Insert new node at end of list
    prev->next = n;
}

// Add symbol to table, overwrites matching key values
void hashmap_add(HashMap *map, void *data, char *key)
{
    int h = hash(key);
    size_t index = (size_t)(h & (uint64_t)(map->size - 1));
    TableNode **index_node = map->table + index;
    insert(index_node, data, key);
}

// Return entry if it exists else null
void *hashmap_get(HashMap *map, char *key)
{
    int h = hash(key);
    size_t index = (size_t)(h & (uint64_t)(map->size - 1));
    TableNode **node = map->table + index;
    return find(*node, key);
}

HashMap *hashmap_new(void)
{
    HashMap *map = my_malloc(sizeof(HashMap));
    TableNode **table = calloc(TABLE_SIZE, sizeof(TableNode *));

    map->table = table;
    map->size = TABLE_SIZE;

    return map;
}

// Creates a new shallow clone of table
HashMap *symtab_clone(HashMap *symtab)
{
    HashMap *clone = hashmap_new();
    for (size_t i = 0; i < symtab->size; i++)
    {
        TableNode *curr = symtab->table[i];

        while (curr)
        {
            hashmap_add(clone, curr->data, curr->key);

            curr = curr->next;
        }
    }
    return clone;
}

void hashmap_free(HashMap *map)
{
    for (size_t i = 0; i < map->size; i++)
    {
        TableNode *tmp;
        TableNode *curr = map->table[i];

        while (curr)
        {
            tmp = curr->next;

            // Free Table Node
            free(curr);

            curr = tmp;
        }
    }
    free(map->table);
    free(map);
}

void type_env_init(HashMap *type_env)
{
    hashmap_add(type_env, make_type_env_entry("int", TS_INT), "int");
    hashmap_add(type_env, make_type_env_entry("void", TS_VOID), "void");
    hashmap_add(type_env, make_type_env_entry("char", TS_CHAR), "char");
    hashmap_add(type_env, make_type_env_entry("float", TS_FLOAT), "float");
    hashmap_add(type_env, make_type_env_entry("double", TS_DOUBLE), "double");
    hashmap_add(type_env, make_type_env_entry("long", TS_LONG), "long");
}

void type_env_free(HashMap *map)
{
    for (size_t i = 0; i < map->size; i++)
    {
        TableNode *tmp;
        TableNode *curr = map->table[i];

        while (curr)
        {
            tmp = curr->next;

            // Free Table Node
            free(curr);

            curr = tmp;
        }
    }
    free(map->table);
    free(map);
}
