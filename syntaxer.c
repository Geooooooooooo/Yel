#include "syntaxer.h"
#include "parser.h"

#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]

typedef struct __YelToken {
    YelTokenType type;
    char* value;
} YelToken;

/*tok_undefined = -1,
    tok_name,
    tok_number,
    tok_string,
    tok_binary_op,
    tok_unary_op,
    tok_op,
    tok_word*/

YelEntities yel_define_next_entity(YelTokens* yel_tokens) {
    register size_t i = yel_tokens->pointer;

    while (i < yel_tokens->length) {
        if (__builtin_strcmp(yel_tokens->value[i], ";") == 0) {
            return en_end;
        }

        // a ...
        if (yel_tokens->type[i] == tok_name) {
            if (__builtin_strcmp(yel_tokens->value[i + 1], ":") == 0) {
                if (yel_tokens->type[i+2] != tok_name && yel_tokens->type[i+2] != tok_number) {
                    printf("Syntax error: after ':' a type or class is expected\n");
                    return en_end;
                }
            }

            // a bin_op
            if (yel_tokens->type[i + 1] == tok_binary_op) {

                // a = ...
                if (__builtin_strcmp(yel_tokens->value[i + 1], "=") == 0) {
                    return en_stmt;
                }

                // a bin_op ...
                else {
                    return en_expr;
                }
            }

            // a op ...
            else if (yel_tokens->type[i + 1] == tok_op) {

                // a ( ...
                if (__builtin_strcmp(yel_tokens->value[i + 1], "(") == 0) {
                    return en_expr;
                }

                else if (__builtin_strcmp(yel_tokens->value[i + 1], ";") == 0) {
                    return en_expr;
                }  
                else if (__builtin_strcmp(yel_tokens->value[i + 1], ":") == 0) {
                    if (yel_tokens->type[i+2] != tok_name && yel_tokens->type[i+2] != tok_number) {
                        printf("Syntax error: after ':' a type or class is expected\n");
                    }

                    return en_stmt;
                }
            }
        }

        // 1 ...
        else if (yel_tokens->type[i] == tok_number) {
            if (__builtin_strcmp(yel_tokens->value[i + 1], "=") == 0) {
                // error
                printf("Syntax error: expression must be non-literal or modifiable\n");
            }

            return en_expr;
        }

        else if (yel_tokens->type[i] == tok_word) {

            // return ...
            if (__builtin_strcmp(yel_tokens->value[i], "return") == 0) {
                return en_stmt;
            }

            // break ...
            else if (__builtin_strcmp(yel_tokens->value[i], "break") == 0) {
                return en_stmt;
            }
        }

        ++i;
    }
}

void yel_gen_parse_tree(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        switch (yel_define_next_entity(yel_tokens)) {
        case (YelEntities)en_stmt: 
            //puts("en_stmt");
            yel_parse_statement(yel_tokens);
            break;
        case (YelEntities)en_decl: 
            puts("en_decl");
            break;
        case (YelEntities)en_expr: 
            //puts("en_expr");
            yel_parse_expression(yel_tokens);
            break;
        case (YelEntities)en_end: 
            puts("en_end");
            break;
        }

        ++yel_tokens->pointer;
    }
}