#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

typedef enum DafTokenType {
    variable,
    number,
    string,
    operator,
    rparentheses,
    lparentheses,
    rbrackets,
    lbrackets,
    rcurlybrace,
    lcurlybrace
} DafTokenType;

typedef struct DafToken {
    DafTokenType token_type;
    char* token_value;
} DafToken;

typedef struct Source {
    char* source_text;
    size_t pointer;
    size_t length;
} Source;

#endif // __DEPENDECIES_H__