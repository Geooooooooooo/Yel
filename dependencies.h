#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum __YelTokenType {
    tok_undefined = -1,
    tok_name,
    tok_number,
    tok_string,
    tok_binary_op,
    tok_unary_op,
    tok_op,
    tok_word
} YelTokenType;

typedef struct __YelTokens {
    size_t length;
    size_t pointer;
    YelTokenType* type;
    char** value;
} YelTokens;

typedef struct __Source {
    char* source_text;
    size_t pointer;
    size_t length;
} Source;

typedef enum __YelEntities {
    en_end = -1,
    en_expr,
    en_stmt,
    en_decl
} YelEntities;

#define __dbreak() printf("debug call in <File: %s:%d>", __FILE__, __LINE__);getchar()
#define print_cur() printf("\ncur = %s\n", cur)

#endif // __DEPENDECIES_H_