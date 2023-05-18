#include "syntaxer.h"

#define next yel_tokens->value[i+1]
#define prev yel_tokens->value[i-1]
#define cur yel_tokens->value[i]

size_t i = 0;
int recursive_descent = 0;
int brakets = 0;

void yel_parse_expression(YelTokens* yel_tokens) {
    while (i < yel_tokens->length) {
        if (yel_tokens->value[i][0] == ';') {
            return;
        }

        if (cur[0] == '(') {
            ++i;
            ++brakets;
            yel_parse_expression(yel_tokens);
        }
        else if (cur[0] == ')') {
            ++i;
            --brakets;
            return;
        }
        else if (next[0] == '*' || next[0] == '/') {
            if (yel_tokens->value[i+2][0] == '(') {
                printf("%s ", cur);
                size_t tmp_i = i + 1;
                i += 3;
                ++brakets;

                yel_parse_expression(yel_tokens);

                printf("%s ", yel_tokens->value[tmp_i]);
                continue;
            }
            else {
                printf("%s %s %s ", cur, yel_tokens->value[i+2], next);
                i += 2;
            }
        }
        else if(cur[0] == '*' || cur[0] == '/') {
            if (next[0] == '(') {
                size_t tmp_i = i;
                i += 2;
                ++brakets;

                yel_parse_expression(yel_tokens);

                printf("%s ", yel_tokens->value[tmp_i]);
                continue;
            }
            else {
                printf("%s %s ", next, cur);
                ++i;
            }
        }
        else if (next[0] == '+' || next[0] == '-') {
            if (recursive_descent) {
                --recursive_descent;
                return;
            }

            size_t gTmp_i = i+3;

            if (yel_tokens->value[i+2][0] == '(') {
                printf("%s ", cur);
                size_t tmp_i = i + 1;

                i += 3;
                ++brakets;
                yel_parse_expression(yel_tokens);

                if (cur[0] == '*' || cur[0] == '/') {
                    ++recursive_descent;
                    yel_parse_expression(yel_tokens);
                }

                printf("%s ", yel_tokens->value[tmp_i]);
                continue;
            }
            else if (yel_tokens->value[gTmp_i][0] == ';' || yel_tokens->value[gTmp_i][0] == '+' || yel_tokens->value[gTmp_i][0] == '-'|| yel_tokens->value[gTmp_i][0] == ')') {
                printf("%s %s %s ", cur, yel_tokens->value[i+2], next);

                i += 2;
            }
            else if (yel_tokens->value[gTmp_i][0] == '*' || yel_tokens->value[gTmp_i][0] == '/') {
                printf("%s ", cur);

                size_t tmp_i = i + 1;
                i += 2;
                ++recursive_descent;

                yel_parse_expression(yel_tokens);

                printf("%s ", yel_tokens->value[tmp_i]);
            }
        }
        // cur = + or -
        else if(cur[0] == '+' || cur[0] == '-') {
            if (recursive_descent) {
                --i;
                --recursive_descent;
                return;
            }
            // expr + ( expr )
            if (next[0] == '(') {
                size_t tmp_i = i;
                i += 2;
                ++brakets;
                yel_parse_expression(yel_tokens);

                printf("%s ", yel_tokens->value[tmp_i]);
                continue;
            }
            else if (yel_tokens->value[i+2][0] == ';' || yel_tokens->value[i+2][0] == '+' || yel_tokens->value[i+2][0] == '-'|| yel_tokens->value[i+2][0] == ')') {
                printf("%s %s ", next, cur);
                ++i;
            }
            else if (yel_tokens->value[i+2][0] == '*' || yel_tokens->value[i+2][0] == '/') {
                size_t tmp_i = i;
                ++i;
                ++recursive_descent;
                yel_parse_expression(yel_tokens);

                printf("%s ", yel_tokens->value[tmp_i]);
            }
        }
        else if (yel_tokens->type[i] == tok_number) {
            printf("%s ", cur);
        }
        else if (yel_tokens->type[i] == tok_string) {
            printf("string(%s) ", cur);
        }
        else if (yel_tokens->type[i] == tok_name) {
            printf("%s ", cur);
        }

        ++i;
    }
}

void yel_gen_parse_tree(YelTokens* yel_tokens) {
    while (i < yel_tokens->length) {
        if (__builtin_strcmp(yel_tokens->value[i], "=") == 0) {
            printf("assing(%s expr( ", yel_tokens->value[i - 1]);
            
            ++i;
            yel_parse_expression(yel_tokens);

            puts("))");
        }

        ++i;
    }
}
