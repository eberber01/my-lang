
#ifndef UTIL_H
#define UTIL_H
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

str2int_errno str2int(int *out, char *s, int base);

typedef struct String{
    char* str;    
    size_t length;
    size_t size;
} String;

struct String* string_new();
void string_append(struct String* string, char c);
void str_resize();

void* my_malloc(size_t bytes);
void* my_realloc(void* ptr, size_t bytes);

#endif
