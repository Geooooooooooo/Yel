#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

typedef enum DafTokenType {
    tok_undefined = -1,
    tok_name,
    tok_number,
    tok_string,
    tok_binary_op,
    tok_unary_op,
    tok_op
} DafTokenType;

typedef struct Source {
    char* source_text;
    size_t pointer;
    size_t length;
} Source;

#endif // __DEPENDECIES_H_
