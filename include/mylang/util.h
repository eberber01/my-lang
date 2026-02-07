#ifndef UTIL_H
#define UTIL_H
#include "mylang/arena.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

extern Arena default_arena;
extern Arena *context_arena;

#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
    } while (0)
#endif

#define VECTOR_INIT_SIZE 10
typedef enum
{
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

str2int_errno str2int(int *out, char *s, int base);

typedef struct Vector
{
    void **array;
    size_t size;
    size_t length;
} Vector;

typedef struct String
{
    Vector *vector;
    size_t length;
} String;

char *int_to_str(int num, int size);

Vector *vector_new(void);

void vector_free(Vector *);

void vector_push(Vector *vector, void *ptr);

void string_free(String *string);

String *string_new(void);

void string_append(String *string, char c);

char *as_str(String *string);

void *vector_get(Vector *vector, size_t index);

void *my_malloc(size_t bytes);

void *my_realloc(void *ptr, size_t bytes);

String *string(char *str);

String *string_clone(String *string);

bool string_eq(String *string, char *cmp);

char *str_clone(char *s);

void *context_alloc(size_t size);

#endif
