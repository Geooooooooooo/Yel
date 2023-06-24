#ifndef __DEPENDECIES_H__
#define __DEPENDECIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long   OPCODEWORD;

typedef struct __OPCODES {
    OPCODEWORD* codes;
    size_t len;
} OPCODES;

typedef enum __YelTokenType {
    tok_undefined   = -1,
    tok_name = 100, // obj
    tok_number_int, // const numeric integer literal - CNIL
    tok_number_flt, // const numeric floating literal - CNFL
    tok_string,     // const string literal - CSL
    tok_bool,       // const bool literal True or False

    tok_binary_op = 400,
    tok_binary_op_pow,                  // **
    tok_binary_op_div,                  // /
    tok_binary_op_mul,                  // *
    tok_binary_op_percent,              // %
    tok_binary_op_plus,                 // +
    tok_binary_op_minus,                // -
    tok_binary_op_rsh,                  // >>
    tok_binary_op_lsh,                  // <<
    tok_binary_op_more,                 // >
    tok_binary_op_less,                 // <
    tok_binary_op_more_eq,              // >=
    tok_binary_op_less_eq,              // <=
    tok_binary_op_eq,                   // ==
    tok_binary_op_not_eq,               // !=
    tok_binary_op_and,                  // &
    tok_binary_op_or,                   // |
    tok_binary_op_log_and,              // &&
    tok_binary_op_log_or,               // ||
    tok_binary_op_div_assign = 422,     // /=
    tok_binary_op_mul_assign,           // *=
    tok_binary_op_percent_assign,       // %=
    tok_binary_op_plus_assign,          // +=
    tok_binary_op_minus_assign,         // -=
    tok_binary_op_rsh_assign,           // >>=
    tok_binary_op_lsh_assign,           // <<=
    tok_binary_op_and_assign = 435,     // &=
    tok_binary_op_or_assign,            // |=
    tok_binary_op_pow_assign,           // **=
    tok_binary_op_assign,               // =

    tok_unary_op = 500,
    tok_unary_op_pos    = tok_binary_op_plus,
    tok_unary_op_neg    = tok_binary_op_minus,
    tok_unary_op_not = 501,     // !
    tok_unary_op_inc,           // ++obj or obj++
    tok_unary_op_dec,           // --obj or obj--

    tok_op = 600, 
    tok_op_lpar,        // (
    tok_op_rpar,        // )
    tok_op_flbrk,       // {
    tok_op_frbrk,       // }
    tok_semicolon,      // ;
    tok_colon,          // :
    tok_comma,          // ,
    tok_op_follow,     // ->

    tok_word = 700,
    tok_word_func,      // func
    tok_word_return,    // return
    tok_word_defer,     // defer
    tok_word_break,     // break
    tok_word_noreturn,  // noreturn
    tok_word_Int,       // Int
    tok_word_Flt,       // Flt
    tok_word_Str,       // Str
    tok_word_Any,       // Any
    tok_word_Bool,      // Bool
    tok_word_if,        // if
    tok_word_else,      // else
    tok_word_while,     // while

    tok_$,              // end of expression
    tok_en_expr,
    tok_en_stmt,
    tok_en_decl,
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

#define print_cur() printf("\ncur = %s\n", _CurVal)
#define print_tokens() puts("tokens = {");\
    for (size_t i = 0; i < token_array.length; i++) {\
        printf("   [%d, '%s', %lu:%lu]\n", token_array.type[i], token_array.value[i], token_array.line[i], token_array.start_symbol[i]);\
    } puts("}")

#define _CurType    yel_tokens->type[yel_tokens->pointer]
#define _NextType   yel_tokens->type[yel_tokens->pointer+1]
#define _CurVal     yel_tokens->value[yel_tokens->pointer]
#define _NextVal    yel_tokens->value[yel_tokens->pointer+1]

#define RET_CODE_ERROR  0x1
#define RET_CODE_OK     0x0

// YVM OPCODES

#define OP_HALT             (OPCODEWORD)0
#define LOAD_VALUE          (OPCODEWORD)1
#define LOAD_CONST          (OPCODEWORD)2
#define POP_VALUE           (OPCODEWORD)3
#define DUP_VALUE           (OPCODEWORD)4

#define UNARY_POS           (OPCODEWORD)5
#define UNARY_NEG           (OPCODEWORD)6
#define UNARY_NOT           (OPCODEWORD)7
#define UNARY_INC           (OPCODEWORD)8
#define UNARY_DEC           (OPCODEWORD)9

#define BYNARY_OP           (OPCODEWORD)10
#define BYNARY_POW          (OPCODEWORD)11
#define BYNARY_DIV          (OPCODEWORD)12
#define BYNARY_MUL          (OPCODEWORD)13
#define BYNARY_MOD          (OPCODEWORD)14
#define BYNARY_ADD          (OPCODEWORD)15
#define BYNARY_SUB          (OPCODEWORD)16
#define BYNARY_RSH          (OPCODEWORD)17
#define BYNARY_LSH          (OPCODEWORD)18
#define BYNARY_MORE         (OPCODEWORD)19
#define BYNARY_LESS         (OPCODEWORD)20
#define BYNARY_MORE_EQ      (OPCODEWORD)21
#define BYNARY_LESS_EQ      (OPCODEWORD)22
#define BYNARY_EQ           (OPCODEWORD)23
#define BYNARY_NOT_EQ       (OPCODEWORD)24
#define BYNARY_AND          (OPCODEWORD)25
#define BYNARY_OR           (OPCODEWORD)26
#define BYNARY_LOGICAL_AND  (OPCODEWORD)27
#define BYNARY_LOGICAL_OR   (OPCODEWORD)28

#define OP_JUMP_TO          (OPCODEWORD)29
#define OP_JUMP_ZERO        (OPCODEWORD)30

#define OP_CALL             (OPCODEWORD)31
#define OP_RET              (OPCODEWORD)32
#define OP_BRK              (OPCODEWORD)33
#define OP_STORE            (OPCODEWORD)34

#endif // __DEPENDECIES_H_