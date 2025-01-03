#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include "util.h"


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
str2int_errno str2int(int *out, char *s, int base) { char *end; if (s[0] == '\0' || isspace(s[0]))
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


void* my_malloc(size_t bytes){

    void* ptr = malloc(bytes);
    if(ptr == NULL){
        perror("Failed to allocate memory.");
        exit(1);
    };
    return ptr;
}

void* my_realloc(void* ptr,size_t bytes){

    void* new_ptr = realloc(ptr, bytes);
    if(new_ptr == NULL){
        perror("Failed to allocate memory.");
        exit(1);
    };
    return new_ptr;
}


void* vector_new(){
    //Allocate struct
    Vector* vector = my_malloc(sizeof(Vector));

    //Allocate inital size
    void** array = my_malloc(sizeof(void*) * 10);

    vector->array = array;
    vector->size = 10;
    vector->length = 0;

    return vector;
}
void vector_push(Vector* vector, void* ptr){

    if(vector->length >= (vector->size)){
      vector->size *= 2;
      void** new_array = my_realloc(vector->array, sizeof(void*) * (vector->size));
      vector->array = new_array;
      new_array = NULL;
    }
    vector->array[vector->length] = ptr;
    vector->length += 1;
}


String* string_new(){

    //Allocate struct
    String* string = my_malloc(sizeof(String));
    //Allocate inital string
    Vector* vector = vector_new();

    //Null terminate
    *str = '\0';

    //Set data 
    string->size = 10;
    string->length =0; 
    string->str =str;
    
    return string;
}


void string_append(String* string, char c){


    if(string->length >= (string->size - 1)){
      char* new_string = my_realloc(string, sizeof(char) * (2 * string->size));

      string->str = new_string;
      new_string = NULL;
    }

    //Overwrite Null Character
    (string->str)[string->length] = c;

    //Terminate at next spot
    (string->length)++;
    (string->str)[string->length] = '\0';

    //Set new size
    string->size = string->size * 2;
}

