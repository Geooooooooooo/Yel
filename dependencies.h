#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum __YelTokenType {
    tok_undefined   = -1,
    tok_name        = 100, // obj
    tok_number      = 200, // const numeric literal - CNL
    tok_string      = 300, // const string literal - CSL

    tok_binary_op               = 400,
    tok_binary_op_plus          = 401, // +
    tok_binary_op_minus         = 402, // -
    tok_binary_op_plus_assign   = 403, // +=
    tok_binary_op_minus_assign  = 404, // -=
    tok_binary_op_div           = 405, // /
    tok_binary_op_mul           = 406, // *
    tok_binary_op_div_assign    = 407, // /=
    tok_binary_op_mul_assign    = 408, // *=
    tok_binary_op_more          = 409, // >
    tok_binary_op_less          = 410, // <
    tok_binary_op_more_eq       = 411, // >=
    tok_binary_op_less_eq       = 412, // <=
    tok_binary_op_assign        = 413, // =
    tok_binary_op_eq            = 414, // ==
    tok_binary_op_not_eq        = 415, // !=
    tok_binary_op_log_and       = 416, // &
    tok_binary_op_and           = 417, // &&
    tok_binary_op_log_or        = 418, // |
    tok_binary_op_or            = 419, // ||
    tok_binary_op_rsh           = 420, // >>
    tok_binary_op_lsh           = 421, // <<
    tok_binary_op_rsh_assign    = 420, // >>=
    tok_binary_op_lsh_assign    = 421, // <<=
    tok_binary_op_log_and_assign= 422, // &=
    tok_binary_op_log_or_assign = 423, // |=
    tok_binary_op_pow           = 424, // **
    tok_binary_op_percent       = 423, // %
    tok_binary_op_percent_assign= 423, // %=

    tok_unary_op                = 500,
    tok_unary_op_pos            = tok_binary_op_plus,
    tok_unary_op_neg            = tok_binary_op_minus,
    tok_unary_op_not            = 503, // !
    tok_unary_op_inc            = 504, // ++obj or obj++
    tok_unary_op_dec            = 505, // --obj or obj--

    tok_op          = 600, 
    tok_op_lpar     = 601, // (
    tok_op_rpar     = 602, // )
    tok_op_flbrk    = 603, // {
    tok_op_frbrk    = 604, // }
    tok_semicolon   = 605, // ;
    tok_colon       = 606, // :
    tok_comma       = 607, // ,
    tok_op_follow   = 608, // ->

    tok_word        = 700
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