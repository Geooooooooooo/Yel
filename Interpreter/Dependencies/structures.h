#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long OPCODEWORD;
typedef long long SIZE_REF;

typedef struct __YelByteCode {
    OPCODEWORD* opcode;
    long long   len;
} YelByteCode;

typedef enum __YelTokenType {
    tok_undefined   = -1,
    tok_name = 100, // obj
    tok_number_int, // const numeric integer literal - CNIL
    tok_number_flt, // const numeric floating literal - CNFL
    tok_string,     // const string literal - CSL
    tok_bool_true,
    tok_bool_false,
    tok_bool,       // const bool literal True or False

    tok_binary_op = 400,
    tok_binary_op_pow,                  // ***
    tok_binary_op_div,                  // /
    tok_binary_op_mul,                  // *
    tok_binary_op_percent,              // %
    tok_binary_op_plus,                 // +
    tok_binary_op_minus,                // -
    tok_binary_op_rsh,                  // >>
    tok_binary_op_lsh,                  // <<
    tok_binary_op_and,                  // &
    tok_binary_op_xor,                  // ^
    tok_binary_op_or,                   // |
    tok_binary_op_more,                 // >
    tok_binary_op_less,                 // <
    tok_binary_op_more_eq,              // >=
    tok_binary_op_less_eq,              // <=
    tok_binary_op_eq,                   // ==
    tok_binary_op_not_eq,               // !=
    tok_binary_op_log_and,              // and
    tok_binary_op_log_or,               // or
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
    tok_binary_op_xor_assign,           // ^=
    tok_binary_op_assign,               // =

    tok_unary_op = 500,
    tok_unary_op_pos    = tok_binary_op_plus,
    tok_unary_op_neg    = tok_binary_op_minus,
    tok_unary_op_log_not = 501,     // not
    tok_unary_op_inc,           // ++obj or obj++
    tok_unary_op_dec,           // --obj or obj--
    tok_unary_op_not,           // ~

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
    tok_word_if,        // if
    tok_word_else,      // else
    tok_word_while,     // while
    tok_word_continue,  // continue
    tok_word_Int,
    tok_word_Flt,
    tok_word_Bool,
    tok_word_Str,

    tok_$,              // end of expression
    tok_en_expr,
    tok_en_stmt,
    tok_en_decl,
} YelTokenType;

typedef enum __YelParsingEntities {
    empty = 800,
    undefined,
    type_annotation,

    // statment
    stmt,
    stmt_name_decl,
    stmt_name_decl_enum,
    stmt_assign,
    stmt_return,
    stmt_break,
    stmt_continue,
    stmt_if,
    stmt_if_else,
    stmt_curly_bracket,
    stmt_while,

    // declarations
    decl,
    decl_func,

    // expressions
    expr, // = 16,
    expr_name,
    expr_num,
    expr_bool,
    expr_str,
    expr_bin_op,
    expr_unary_op,
    expr_enumeration,
    expr_func_decl,
    expr_brackets,
    expr_func_call,

    bin_op,
    unary_op,
    assign_op,

} YelParsingEntities;

typedef struct __YelTokens {
    _Bool           error;
    size_t          length;
    size_t          pointer;
    size_t*         line;            // trace 
    size_t*         start_symbol;   // trace
    YelTokenType*   type;
    char**          value;
    char*           file_name;
    struct __Source* src_ptr;
} YelTokens;

typedef struct __YelSource {
    char*   source_text;
    char*   file_name;        // trace   
    size_t  pointer;
    size_t  length;
} YelSource;

typedef struct __YelConstant {
    int         type;
    OPCODEWORD  ref;     // ref to value
} YelConstant;

typedef struct __YelVariable {
    char*       name;
    OPCODEWORD  ref;     // to YelConstant
} YelVariable;

typedef struct __YelFunction {
    int             returt_type;
    int             argc;
    SIZE_REF*       args;
    OPCODEWORD      start;
} YelFunction;