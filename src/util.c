#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"

/* Convert string s to int out.
 *
 * @param[out] out The converted int. Cannot be NULL.
 *
 * @param[in] s Input string to be converted.
 *
 *     The format is the same as strtol,
 *     except that the following are inconvertible:
 *
 *     - empty string
 *     - leading whitespace
 *     - any trailing characters that are not part of the number
 *
 *     Cannot be NULL.
 *
 * @param[in] base Base to interpret string in. Same range as strtol (2 to 36).
 *
 * @return Indicates if the operation succeeded, or why it failed.
 *
 * CREDIT: https://stackoverflow.com/a/12923949
 */
str2int_errno str2int(int *out, char *s, int base)
{
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
        return STR2INT_OVERFLOW;
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
        return STR2INT_UNDERFLOW;
    if (*end != '\0')
        return STR2INT_INCONVERTIBLE;
    *out = l;
    return STR2INT_SUCCESS;
}

char *str_clone(char* s)
{
    char* str = (char*)my_malloc(strlen(s) + 1);
    strcpy(str, s);
    return str;
}

void *my_malloc(size_t bytes)
{
    void *ptr = malloc(bytes);
    if (ptr == NULL)
    {
        perror("Failed to allocate memory.");
        exit(1);
    };
    return ptr;
}

void *my_realloc(void *ptr, size_t bytes)
{
    void *new_ptr = realloc(ptr, bytes);
    if (new_ptr == NULL)
    {
        perror("Failed to allocate memory.");
        exit(1);
    };
    return new_ptr;
}

Vector *vector_new(void)
{
    // Allocate struct
    Vector *vector = my_malloc(sizeof(Vector));

    // Allocate inital size
    void **array = my_malloc(sizeof(void *) * VECTOR_INIT_SIZE);

    vector->array = array;
    vector->size = 10;
    vector->length = 0;

    return vector;
}
void vector_push(Vector *vector, void *ptr)
{
    if (vector->length >= (vector->size))
    {
        vector->size *= 2;
        void **new_array = my_realloc(vector->array, sizeof(void *) * (vector->size));
        vector->array = new_array;
        new_array = NULL;
    }
    vector->array[vector->length] = ptr;
    vector->length += 1;
}

void vector_set(Vector *vector, size_t index, void *ptr)
{
    if (index >= vector->length)
    {
        printf("Index out of range. %zu", index);
        exit(1);
    }
    vector->array[index] = ptr;
}

void *vector_get(Vector *vector, size_t index)
{
    if (index >= vector->length)
    {
        printf("Index out of range. %zu", index);
        exit(1);
    }
    return vector->array[index];
}

// Caller must free entries in vector
// before calling this function
void vector_free(Vector *vector)
{
    free(vector->array);
    free(vector);
}

void string_free(String *string)
{
    for(size_t i=0; i < string->length; i++)
        free(vector_get(string->vector, i));
    vector_free(string->vector);
    free(string);
}

String *string_new(void)
{
    // Allocate struct
    String *string = my_malloc(sizeof(String));
    // Allocate inital string
    Vector *vector = vector_new();

    string->length = 0;
    string->vector = vector;

    return string;
}

String *string(char *str)
{
    String *s = string_new();
    int i = 0;
    while (str[i] != '\0')
    {
        string_append(s, str[i]);
        i++;
    }
    return s;
}

String *string_clone(String *string)
{
    String *s = string_new();
    char *c;
    for (size_t i = 0; i < string->length; i++)
    {
        c = (char *)vector_get(string->vector, i);
        string_append(s, *c);
    }
    return s;
}

bool string_eq(String *string, char *cmp)
{
    if (string->length != strlen(cmp))
        return false;
    char *c;
    for (size_t i = 0; i < string->length; i++)
    {
        c = (char *)vector_get(string->vector, i);
        if (*c != cmp[i])
            return false;
    }
    return true;
}

void string_append(String *string, char c)
{
    char *n = my_malloc(sizeof(char));
    *n = c;

    vector_push(string->vector, (void *)n);
    string->length += 1;
}

char *as_str(String *string)
{
    char *s = my_malloc(sizeof(char) * string->length + 1);
    for (size_t i = 0; i < string->length; i++)
    {
        s[i] = *((char *)vector_get(string->vector, i));
    }
    s[string->length] = '\0';
    return s;
}

char *int_to_str(int num, int size)
{
    char *str = my_malloc((sizeof(char) * size) + 1);
    snprintf(str, size, "%d", num);
    return str;
}
