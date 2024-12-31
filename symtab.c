#include "lex.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define TABLE_SIZE 100

typedef struct SymTabEntry{
    TokenType type;
    char* value;
    char* key;
} SymTabEntry;

typedef struct TableNode{
    struct SymTabEntry* data;
    struct TableNode* next;
}TableNode;

typedef struct SymTab{
    size_t size;
    struct TableNode** map;
} SymTab;

struct SymTabEntry* make_symtab_entry(char* key, char* value, TokenType type){
    struct SymTabEntry* entry = my_malloc(sizeof(SymTabEntry));
    entry->key = key;
    entry->value = value;
    entry->type = type;
    return entry;
}

struct TableNode* make_node(struct TableNode* next, struct SymTabEntry* data){
    struct TableNode* node = my_malloc(sizeof(TableNode));
    node->data = data; 
    node->next = next; 
    return node;
}

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
// https://benhoyt.com/writings/hash-table-in-c/
static uint64_t hash(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}


struct SymTabEntry* find(struct TableNode* node, char* key){
    struct TableNode* curr = node;
    while(curr != NULL){
        if(!strcmp(node->data->key, key)){
            return curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}

// Insert node at next empty spot
void insert(TableNode** node, struct SymTabEntry* entry){ 
    struct TableNode* n = make_node(NULL, entry);

    //Point to new node if first hash

    if(*node == NULL){
        *node = n;
        return;
    }

    struct TableNode* curr = *node;

    while(curr->next != NULL){
        curr = curr->next;
    }

    curr->next = n;
}

void symtab_add(struct SymTab* table,struct SymTabEntry* entry){
    int h = hash(entry->key);
    size_t index = (size_t)(h & (uint64_t)(table->size - 1));
    struct TableNode** node = table->map + index;
    insert(node, entry);
}

//Return entry if it exists else null
struct SymTabEntry* symtab_get(struct SymTab* table, char* key){
    int h = hash( key);
    size_t index = (size_t)(h & (uint64_t)(table->size - 1));
    struct TableNode** node = table->map + index;
    return find(*node, key);
}


struct SymTab* symtab_new(){
    struct SymTab* table = my_malloc(sizeof(SymTab));
    struct TableNode** map = my_malloc(sizeof(TableNode*) * TABLE_SIZE);

    table->map = map;
    table->size = TABLE_SIZE;
    return table;
}

