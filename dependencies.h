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
    _Bool error;
    size_t length;
    size_t pointer;
    size_t *line;            // trace 
    size_t* start_symbol;   // trace
    YelTokenType* type;
    char** value;
    char* file_name;

    struct __Source* src_ptr;
} YelTokens;

typedef struct __Source {
    char* source_text;
    char* file_name;        // trace   
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
#define print_tokens() puts("tokens = {");\
    for (size_t i = 0; i < token_array.length; i++) {\
        printf("   [%d, '%s', %lu:%lu]\n", token_array.type[i], token_array.value[i], token_array.line[i], token_array.start_symbol[i]);\
    } puts("}")

#endif // __DEPENDECIES_H_