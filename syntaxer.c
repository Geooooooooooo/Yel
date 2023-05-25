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
    static YelTokenType last_type = en_end;

    while (i < yel_tokens->length) {
        if (__builtin_strcmp(yel_tokens->value[i], ";") == 0) {
            return last_type;
        }

        // tok_name ...
        if (yel_tokens->type[i] == tok_name) {
            // tok_name bin_op
            if (yel_tokens->type[i + 1] == tok_binary_op) {
                // tok_name = ...
                if (__builtin_strcmp(yel_tokens->value[i + 1], "=") == 0) {
                    return en_stmt;
                }

                // tok_name bin_op ...
                
                last_type = en_expr;
                size_t tmp_i = i;

                ++yel_tokens->pointer;
                if (yel_define_next_entity(yel_tokens) == en_expr) {
                    yel_tokens->pointer = i;
                    return en_expr;
                }

                printf("Syntax error: invalid expression\n");
                return en_end;
            }

            // a op ...
            else if (yel_tokens->type[i + 1] == tok_op) {

                // a ( ...
                if (__builtin_strcmp(yel_tokens->value[i + 1], "(") == 0) {
                    return last_type = en_expr;
                }

                else if (__builtin_strcmp(yel_tokens->value[i + 1], ";") == 0) {
                    return last_type = en_expr;
                }  
            }
            else {
                printf("Syntax error: expression is expected: %s\n", yel_tokens->value[i+1]);
                exit(-1);
            }
        }

        // 1 ...
        else if (yel_tokens->type[i] == tok_number) {
            if (__builtin_strcmp(yel_tokens->value[i + 1], "=") == 0) {
                // error
                printf("Syntax error: expression must be non-literal or modifiable\n");
                exit(-1);
            }
            else if (yel_tokens->type[i+1] != tok_binary_op && __builtin_strcmp(yel_tokens->value[i+1], ";") != 0) {
                printf("Syntax error: expression is expected: %s\n", yel_tokens->value[i+1]);
                exit(-1);
            }

            last_type = en_expr;
            size_t tmp_i = i;
            ++yel_tokens->pointer;
            if (yel_define_next_entity(yel_tokens) == en_expr) {
                yel_tokens->pointer = i;
                return en_expr;
            }

            printf("Syntax error: invalid expression\n");
            return en_end;
        }

        else if (yel_tokens->type[i] == tok_word) {

            // return ...
            if (__builtin_strcmp(yel_tokens->value[i], "return") == 0) {
                return last_type = en_stmt;
            }

            // break ...
            else if (__builtin_strcmp(yel_tokens->value[i], "break") == 0) {
                return last_type = en_stmt;
            }
        }
        else if (yel_tokens->type[i] == tok_string) {
            return last_type = en_expr;
        }

        else if(yel_tokens->type[i] == tok_op) {
            if (__builtin_strcmp(yel_tokens->value[i], "(") == 0) {
                last_type = en_expr;
                size_t tmp_i = i;
                ++yel_tokens->pointer;
                if (yel_define_next_entity(yel_tokens) == en_expr) {
                    yel_tokens->pointer = i;
                    return en_expr;
                }

                printf("Syntax error: invalid expression\n");
                return en_end;
            }
        }

        ++i;
    }
}

void yel_gen_parse_tree(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        switch (yel_define_next_entity(yel_tokens)) {
        case (YelEntities)en_stmt: 
            yel_parse_statement(yel_tokens);
            recursive_descent = 0;
            break;
        case (YelEntities)en_decl: 
            break;
        case (YelEntities)en_expr: 
            yel_parse_expression(yel_tokens);
            recursive_descent = 0;
            break;
        case (YelEntities)en_end: 
            break;
        }

        ++yel_tokens->pointer;
    }
}