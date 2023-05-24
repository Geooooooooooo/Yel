#include "parser.h"
#include "syntaxer.h"

#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]


void yel_parse_statement(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (yel_tokens->type[yel_tokens->pointer] == tok_name) {
            if (__builtin_strcmp(yel_tokens->value[yel_tokens->pointer + 1], "=") == 0) {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;

                if (yel_define_next_entity(yel_tokens) != en_expr) {
                    // error
                    printf("Syntax error: expected expression after '='\n");
                }

                yel_parse_expression(yel_tokens);
                printf("store %s\n", yel_tokens->value[tmp_i]);

                return;
            }
        }

        else if (yel_tokens->type[yel_tokens->pointer] == tok_word) {
            if (__builtin_strcmp(yel_tokens->value[yel_tokens->pointer + 1], "return") == 0) {
                YelEntities next_en = yel_define_next_entity(yel_tokens);

                if (next_en != en_expr && next_en != en_end) {
                    // error
                    printf("Syntax error: expected expression or ';' after return\n");
                    return;
                }

                yel_parse_expression(yel_tokens);
                printf("ret\n");
                return;
            }
            else if (__builtin_strcmp(yel_tokens->value[yel_tokens->pointer + 1], "break") == 0) {
                if (yel_define_next_entity(yel_tokens) != en_end) {
                    // error
                    printf("Syntax error: expected ';' after break\n");
                    return;
                }

                printf("break\n");
                return;
            }
        }
        ++yel_tokens->pointer;
    }
}

void yel_parse_expression(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (yel_tokens->value[yel_tokens->pointer][0] == ';') {
            if (brakets) {
                printf("Syntax error: expected ')'\n");
                exit(-1);
            }

            recursive_descent = 0;
            return;
        }

        if (yel_tokens->type[yel_tokens->pointer] == tok_name && next[0] == '(') {
            size_t tmp_i = yel_tokens->pointer;

            yel_tokens->pointer += 2;
            ++brakets;

            int tmp_recursive_descent = recursive_descent;
            recursive_descent = 0;

            yel_parse_expression(yel_tokens);

            recursive_descent = tmp_recursive_descent;

            printf("call %s\n", yel_tokens->value[tmp_i]);

            if (recursive_descent) {
                --recursive_descent;
                return;
            }
            continue;
        }
        else if (cur[0] == '(') {
            ++yel_tokens->pointer;
            ++brakets;
            yel_parse_expression(yel_tokens);

            continue;
        }
        else if (cur[0] == ')') {
            if (brakets == 0) {
                printf("Syntax error: expected '('\n");
                exit(-1);
            }

            ++yel_tokens->pointer;
            --brakets;
            return;
        }
        else if (next[0] == '*' || next[0] == '/') {
            if (yel_tokens->value[yel_tokens->pointer+2][0] == ';') {
                printf("Syntax error: expected expression after '%s'\n", yel_tokens->value[yel_tokens->pointer+1]);
                exit(-1);
            }

            if (yel_tokens->type[yel_tokens->pointer+2] == tok_name && yel_tokens->value[yel_tokens->pointer+3][0] == '(') {
                printf("push %s\n", cur);

                size_t tmp_i = yel_tokens->pointer + 1;
                ++recursive_descent;
                yel_tokens->pointer += 2;
                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
                continue;
            }
            else if (yel_tokens->value[yel_tokens->pointer+2][0] == '(') {
                printf("push %s\n", cur);
                size_t tmp_i = yel_tokens->pointer + 1;
                yel_tokens->pointer += 3;
                ++brakets;

                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
                continue;
            }
            else {
                printf("push %s\npush %s\n%s\n", cur, yel_tokens->value[yel_tokens->pointer+2], next);
                yel_tokens->pointer += 2;
            }
        }
        else if(cur[0] == '*' || cur[0] == '/') {
            if (yel_tokens->value[yel_tokens->pointer+1][0] == ';') {
                printf("Syntax error: expected expression after '%s'\n", yel_tokens->value[yel_tokens->pointer]);
                exit(-1);
            }

            if (yel_tokens->type[yel_tokens->pointer+1] == tok_name && yel_tokens->value[yel_tokens->pointer+2][0] == '(') {
                size_t tmp_i = yel_tokens->pointer;
                ++yel_tokens->pointer;
                ++recursive_descent;
                
                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
                continue;
            }
            else if (next[0] == '(') {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;
                ++brakets;

                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
                continue;
            }
            else {
                printf("push %s\n%s\n", next, cur);
                ++yel_tokens->pointer;
            }
        }
        else if (next[0] == '+' || next[0] == '-') {
            if (recursive_descent) {
                --recursive_descent;
                return;
            }

            if (yel_tokens->value[yel_tokens->pointer+2][0] == ';') {
                printf("Syntax error: expected expression after '%s'\n", yel_tokens->value[yel_tokens->pointer+1]);
                exit(-1);
            }

            register size_t gTmp_i = yel_tokens->pointer+3;

            if (yel_tokens->value[gTmp_i][0] == '(') {
                size_t tmp_i = yel_tokens->pointer + 1;
                printf("push %s\n", cur);

                yel_tokens->pointer += 2;
                ++recursive_descent;
                yel_parse_expression(yel_tokens);


                printf("%s\n", yel_tokens->value[tmp_i]);
                continue;
            }
            else if (yel_tokens->value[yel_tokens->pointer+2][0] == '(') {
                printf("push %s\n", cur);
                size_t tmp_i = yel_tokens->pointer + 1;

                yel_tokens->pointer += 3;
                ++brakets;
                yel_parse_expression(yel_tokens);


                if (cur[0] == '*' || cur[0] == '/') {
                    ++recursive_descent;
                    yel_parse_expression(yel_tokens);
                }

                printf("%s\n", yel_tokens->value[tmp_i]);
                ++yel_tokens->pointer;
                continue;
            }
            else if (yel_tokens->value[gTmp_i][0] == ';' || yel_tokens->value[gTmp_i][0] == '+' || yel_tokens->value[gTmp_i][0] == '-'|| yel_tokens->value[gTmp_i][0] == ')') {
                printf("push %s \npush %s\n%s\n", cur, yel_tokens->value[yel_tokens->pointer+2], next);

                yel_tokens->pointer += 2;
            }
            else if (yel_tokens->value[gTmp_i][0] == '*' || yel_tokens->value[gTmp_i][0] == '/') {
                printf("push %s\n", cur);

                size_t tmp_i = yel_tokens->pointer + 1;
                yel_tokens->pointer += 2;
                ++recursive_descent;

                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
            }
        }
        // cur = + or -
        else if(cur[0] == '+' || cur[0] == '-') {
            if (recursive_descent) {
                --yel_tokens->pointer;
                --recursive_descent;
                return;
            }

            if (yel_tokens->value[yel_tokens->pointer+1][0] == ';') {
                printf("Syntax error: expected expression after '%s'\n", yel_tokens->value[yel_tokens->pointer]);
                exit(-1);
            }

            if (yel_tokens->value[yel_tokens->pointer+2][0] == '(') {
                size_t tmp_i = yel_tokens->pointer;
                ++yel_tokens->pointer;
                ++recursive_descent;

                // read function call
                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
                continue;
            }
            // expr + ( expr )
            else if (next[0] == '(') {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;
                ++brakets;
                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
                //getchar();
                continue;
            }
            else if (yel_tokens->value[yel_tokens->pointer+2][0] == ';' || yel_tokens->value[yel_tokens->pointer+2][0] == '+' || yel_tokens->value[yel_tokens->pointer+2][0] == '-'|| yel_tokens->value[yel_tokens->pointer+2][0] == ')') {
                printf("push %s\n%s\n", next, cur);
                ++yel_tokens->pointer;
            }
            else if (yel_tokens->value[yel_tokens->pointer+2][0] == '*' || yel_tokens->value[yel_tokens->pointer+2][0] == '/') {
                size_t tmp_i = yel_tokens->pointer;
                ++yel_tokens->pointer;
                ++recursive_descent;
                yel_parse_expression(yel_tokens);

                printf("%s\n", yel_tokens->value[tmp_i]);
            }
        }
        else if (yel_tokens->type[yel_tokens->pointer] == tok_number) {
            printf("push %s\n", cur);
        }
        else if (yel_tokens->type[yel_tokens->pointer] == tok_name) {
            printf("push %s\n", cur);
        }
        else if (yel_tokens->type[yel_tokens->pointer] == tok_string) {
            printf("push '%s'\n", cur);
        }

        ++yel_tokens->pointer;
    }
}
