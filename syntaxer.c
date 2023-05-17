#include "syntaxer.h"

#define next yel_tokens->value[i+1]
#define prev yel_tokens->value[i-1]
#define cur yel_tokens->value[i]

size_t i = 0;
int rec_mul_div = 0;
_Bool last_mul_div = 0;

void yel_parse_expression(YelTokens* yel_tokens) {
    while (i < yel_tokens->length) {
        if (yel_tokens->value[i][0] == ';') {
            return;
        }

        if (next[0] == '*' || next[0] == '/') {
            puts(cur);
            puts(yel_tokens->value[i+2]);
            puts(next);

            i += 2;
        }
        else if(cur[0] == '*' || cur[0] == '/') {
            puts(next);
            puts(cur);

            ++i;
        }
        else if (next[0] == '+' || next[0] == '-') {
            if (rec_mul_div) {
                --rec_mul_div;
                return;
            }

            if (yel_tokens->value[i+3][0] == ';' || yel_tokens->value[i+3][0] == '+' || yel_tokens->value[i+3][0] == '-') {
                puts(cur);
                puts(yel_tokens->value[i+2]);
                puts(next);

                i += 2;
                // i += 3;
            }
            else if (yel_tokens->value[i+3][0] == '*' || yel_tokens->value[i+3][0] == '*') {
                puts(cur);
                size_t tmp_i = ++i;
                ++i;
                ++rec_mul_div;

                yel_parse_expression(yel_tokens);

                puts(yel_tokens->value[tmp_i]);
            }
        }
        else if(cur[0] == '+' || cur[0] == '-') {
            if (rec_mul_div) {
                --i;
                --rec_mul_div;
                return;
            }

            if (yel_tokens->value[i+2][0] == ';' || yel_tokens->value[i+2][0] == '+' || yel_tokens->value[i+2][0] == '-') {
                puts(next);
                puts(cur);

                ++i;
            }
            else if (yel_tokens->value[i+2][0] == '*' || yel_tokens->value[i+2][0] == '*') {
                puts(prev);
                size_t tmp_i = i;
                ++i;   
                ++rec_mul_div;

                yel_parse_expression(yel_tokens);

                puts(yel_tokens->value[tmp_i]);
            }
        }
        else if(yel_tokens->type[i] == tok_name || yel_tokens->type[i] == tok_number) {
            puts(cur);
        }

        ++i;
    }
}

void yel_gen_parse_tree(YelTokens* yel_tokens) {
    while (i < yel_tokens->length) {
        if (__builtin_strcmp(yel_tokens->value[i], "=") == 0) {
            printf("assing(%s expr( \n", yel_tokens->value[i - 1]);
            
            ++i;
            yel_parse_expression(yel_tokens);

            puts("))");
        }

        ++i;
    }
}