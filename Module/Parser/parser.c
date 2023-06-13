#include "../Parser/parser.h"
#include "../Parser/syntaxer.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"

#define curtype yel_tokens->type[yel_tokens->pointer]
#define nexttype yel_tokens->type[yel_tokens->pointer+1]

#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]
#define bstrcmp(a, b) (__builtin_strcmp(a, b) == 0)

/*typedef struct _Expected {
    int sz;
    YelTokenType t[30];
} Expected;

Expected gExpected;

inline void addExpected(int size, YelTokenType types[]) {
    for (register int i = 0; i < size; i++) {
        gExpected.t[i] = types[i];
    } gExpected.sz = size;
}

inline _Bool inExpected(YelTokenType type) {
    for (register int i = 0; i < gExpected.sz; i++) {
        if (gExpected.t[i] == type) 
            return 1;
    } return 0;
}*/

_Bool unary = 1;
int simple_expr = 0;
int brackets_par = 0;
void yel_parse_expression(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        // UNARY OPERATOR
        if (unary && (
            curtype == tok_binary_op_plus || curtype == tok_binary_op_minus ||
            (curtype >= tok_unary_op_not && curtype <= tok_unary_op_dec)
        )) {
            size_t uo_ptr = yel_tokens->pointer;
            ++simple_expr;
            ++yel_tokens->pointer;

            yel_parse_expression(yel_tokens);

            unary = 0;
            --simple_expr;

            // OP-CODE
            if (yel_tokens->type[uo_ptr] == tok_unary_op_pos)
                puts("pos");
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_neg)
                puts("neg");
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_not)
                puts("not");
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_inc)
                puts("inc");
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_dec)
                puts("dec");

            if (simple_expr) return;
        }

        // (
        else if (curtype == tok_op_lpar) {
            ++brackets_par;
            ++yel_tokens->pointer;
            unary = 1;

            register int tmp_simple_expr = simple_expr;
            simple_expr = 0;
            yel_parse_expression(yel_tokens);
            simple_expr = tmp_simple_expr;

            if (simple_expr) return;
        }
        else if (curtype == tok_op_rpar) {
            if (brackets_par == 0) {
                printf(
                    "Syntax Error: <File '%s'>\n--> expected '(' at %lu:%lu\n|\n|", 
                    yel_tokens->file_name, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );
                yel_print_error(
                    yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );

                yel_tokens->error = 1;
            }

            unary = 0;

            return;
        }

        // **
        else if (nexttype == tok_binary_op_pow) {
            printf("push %s\n", cur);

            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;
                
            yel_parse_expression(yel_tokens);
                
            --simple_expr;

            unary = 0;
            puts("pow");
            if (simple_expr) return;
        }
        else if (curtype == tok_binary_op_pow) {
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            puts("pow");
            if (simple_expr) return;
        }

        // *, /, %
        else if(nexttype == tok_binary_op_div || nexttype == tok_binary_op_mul || nexttype == tok_binary_op_percent) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;
            
            yel_parse_expression(yel_tokens);

            --simple_expr;

            if (nexttype == tok_binary_op_pow) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            // Release version
            // op-code = curtype
            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                puts("div");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                puts("mul");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                puts("mod");
        }
        else if(curtype == tok_binary_op_div || curtype == tok_binary_op_mul || curtype == tok_binary_op_percent) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;
            
            yel_parse_expression(yel_tokens);

            --simple_expr;

            if (nexttype == tok_binary_op_pow) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            // Release version
            // op-code = curtype
            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                puts("div");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                puts("mul");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                puts("mod");

        }
        
        // +, -
        else if (nexttype >= tok_binary_op_plus || nexttype == tok_binary_op_minus && (
                nexttype < tok_binary_op_more && nexttype > tok_binary_op_less_eq)) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_percent) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                puts("add");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                puts("sub");
            
        }
        else if (curtype == tok_binary_op_plus || curtype == tok_binary_op_minus) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_percent) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                puts("add");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                puts("sub");

            if (simple_expr) return;
            continue; 
        }
        
        // <<, >>
        else if (nexttype == tok_binary_op_rsh || nexttype == tok_binary_op_lsh) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_minus) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                puts("rsh");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                puts("lsh");
        }
        else if (curtype == tok_binary_op_rsh || curtype == tok_binary_op_lsh) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_minus) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                puts("rsh");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                puts("lsh");
        }

        // <, <=, >, >=
        else if (nexttype >= tok_binary_op_more && nexttype <= tok_binary_op_less_eq) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_lsh) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_more) 
                puts("more");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less) 
                puts("less");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_more_eq)
                puts("more_eq");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less_eq)
                puts("less_eq");
        }
        else if (curtype >= tok_binary_op_more && curtype <= tok_binary_op_less_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_lsh) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_more) 
                puts("more");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less) 
                puts("less");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_more_eq) 
                puts("more_eq");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less_eq)
                puts("less_eq");
        }

        // ==, !=
        else if (nexttype == tok_binary_op_eq || nexttype == tok_binary_op_not_eq) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_less_eq) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                puts("eq");
            else puts("not_eq");
        }
        else if (curtype == tok_binary_op_eq || curtype == tok_binary_op_not_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_less_eq) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                puts("eq");
            else puts("not_eq");
        }

        // &
        else if (nexttype == tok_binary_op_log_and) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);
            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_not_eq) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            puts("and");
        }
        else if (curtype == tok_binary_op_log_and) {
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_not_eq) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }
            
            puts("and");
        }
        
        // |
        else if (nexttype == tok_binary_op_log_or) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_log_and) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            puts("or");
        }
        else if (curtype == tok_binary_op_log_or) {
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_log_and) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }
            
            puts("or");
        }

        // &&
        else if (nexttype == tok_binary_op_and) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_log_or) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            puts("log_and");
        }
        else if (curtype == tok_binary_op_and) {
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_log_or) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }
            
            puts("log_and");
        }
        
        // ||
        else if (nexttype == tok_binary_op_or) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_and) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }

            puts("log_or");
        }
        else if (curtype == tok_binary_op_or) {
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_and) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }
            
            puts("log_or");
        }
        
        else if(curtype == tok_name) {
            printf("push %s\n", cur);

            if (simple_expr) return;
        }
        else if(curtype == tok_number) {
            printf("push %s\n", cur);

            if (simple_expr) return;
        }
        else if(curtype == tok_string) {
            printf("push %s\n", cur);

            if (simple_expr) return;
        }

        ++yel_tokens->pointer;
    }
}

void yel_parse_statement(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (yel_tokens->type[yel_tokens->pointer] == tok_name) {
            if (__builtin_strcmp(yel_tokens->value[yel_tokens->pointer + 1], "=") == 0) {
                size_t tmp_i = yel_tokens->pointer; 
                yel_tokens->pointer += 2;

                if (yel_define_next_entity(yel_tokens) != en_expr) {
                    printf("Syntax Error: <File '%s'>\n--> expected expression after '=' at %lu:%lu\n|\n|", 
                        yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], yel_tokens->start_symbol[yel_tokens->pointer]);

                    yel_print_error(yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], yel_tokens->start_symbol[yel_tokens->pointer]);

                    yel_tokens->error = 1;
                    return;
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
                    printf("Syntax Error: <File '%s'>\n--> expected expression or ';' after return at %lu:%lu\n|\n|", 
                        yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], yel_tokens->start_symbol[yel_tokens->pointer]);
                        
                    yel_print_error(yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], yel_tokens->start_symbol[yel_tokens->pointer]);

                    yel_tokens->error = 1;
                    return;
                }

                yel_parse_expression(yel_tokens);
                printf("ret\n");
                return;
            }
            else if (__builtin_strcmp(yel_tokens->value[yel_tokens->pointer + 1], "break") == 0) {
                if (yel_define_next_entity(yel_tokens) != en_end) {
                    printf("Syntax Error: <File '%s'>\n--> expected ';' after break at %lu:%lu\n|\n|", 
                        yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], yel_tokens->start_symbol[yel_tokens->pointer]);
                    
                    yel_print_error(yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], yel_tokens->start_symbol[yel_tokens->pointer]);

                    yel_tokens->error = 1;
                    return;
                }

                printf("break\n");
                return;
            }
        }

        else if (yel_tokens->type[yel_tokens->pointer] == tok_op) {
            if (yel_tokens->value[yel_tokens->pointer][0] == '{') {
                yel_parse_expression(yel_tokens);
            }
        }
        ++yel_tokens->pointer;
    }
}
