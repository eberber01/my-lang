#include "lex.h"
#include "util.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define TABLE_SIZE 100


SymTabEntry* make_symtab_entry(char* key, char* value, TokenType type){
    SymTabEntry* entry = my_malloc(sizeof(SymTabEntry));
    entry->key = key;
    entry->value = value;
    entry->type = type;
    return entry;
}

TableNode* make_node(TableNode* next, SymTabEntry* data){
    TableNode* node = my_malloc(sizeof(TableNode));
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


SymTabEntry* find(TableNode* node, char* key){
    TableNode* curr = node;
    while(curr != NULL){
        if(!strcmp(node->data->key, key)){
            return curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}

// Insert node at next empty spot
void insert(TableNode** node, SymTabEntry* entry){ 
    TableNode* n = make_node(NULL, entry);

    //Point to new node if first hash

    if(*node == NULL){
        *node = n;
        return;
    }

    TableNode* curr = *node;

    while(curr->next != NULL){
        curr = curr->next;
    }

    curr->next = n;
}

void symtab_add(SymTab* table,SymTabEntry* entry){
    int h = hash(entry->key);
    size_t index = (size_t)(h & (uint64_t)(table->size - 1));
    TableNode** node = table->map + index;
    insert(node, entry);
}

//Return entry if it exists else null
SymTabEntry* symtab_get(SymTab* table, char* key){
    int h = hash( key);
    size_t index = (size_t)(h & (uint64_t)(table->size - 1));
    TableNode** node = table->map + index;
    return find(*node, key);
}


SymTab* symtab_new(){
    SymTab* table = my_malloc(sizeof(SymTab));
    TableNode** map = calloc(sizeof(TableNode*), TABLE_SIZE);

    table->map = map;
    table->size = TABLE_SIZE;


    return table;
}

void symtab_free(SymTab* table){
    for(int i=0; i <  table->size; i++ ){
        TableNode* prev;
        TableNode* curr = table->map[i];

            while(curr){
                printf("here");
                prev = curr; 
                curr = curr->next;
                free(prev->data);
                free(prev);
            }

    }   
    free(table->map);
    free(table);
}

