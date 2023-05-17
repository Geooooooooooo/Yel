#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum YelTokenType {
    tok_undefined = -1,
    tok_name,
    tok_number,
    tok_string,
    tok_binary_op,
    tok_unary_op,
    tok_op,
    tok_word
} YelTokenType;

typedef struct YelTokens {
    size_t length;
    YelTokenType* type;
    char** value;
} YelTokens;

typedef struct Source {
    char* source_text;
    size_t pointer;
    size_t length;
} Source;

#define __dbreak() printf("debug call in <File: %s:%d>", __FILE__, __LINE__);getchar()

#endif // __DEPENDECIES_H_