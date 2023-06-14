#include "../Parser/parser.h"
#include "../Parser/syntaxer.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"
#include "../Dependencies/dependencies.h"

#define curtype yel_tokens->type[yel_tokens->pointer]
#define nexttype yel_tokens->type[yel_tokens->pointer+1]

#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]
#define bstrcmp(a, b) (__builtin_strcmp(a, b) == 0)

// TODO error handling

_Bool unary = 1;
int simple_expr = 0;
int brackets_par = 0;
void yel_parse_expression(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (curtype == tok_semicolon) {
            if (brackets_par) {
                printf(
                    "Syntax Error: module %s\n--> expected ')' at %lu:%lu\n|\n|", 
                    yel_tokens->file_name, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer-1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer-1]
                );

                yel_tokens->error = 1;
            }

            ++yel_tokens->pointer;

            return;
        }

        // UNARY OPERATOR
        else if (unary && (
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
            if (nexttype == tok_op_rpar) {
                puts("push None");
            }

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
                    "Syntax Error: module %s\n--> expected '(' at %lu:%lu\n|\n|", 
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

            --brackets_par;
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
        else if (nexttype == tok_binary_op_plus || nexttype == tok_binary_op_minus && (nexttype < tok_binary_op_more && nexttype > tok_binary_op_less_eq)) {
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

        // ,
        else if (nexttype == tok_comma) {
            printf("push %s\n", cur);
            if (simple_expr) return;

            yel_tokens->pointer += 2;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_or) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }
        }
        else if (curtype == tok_comma) {
            ++yel_tokens->pointer;
            ++simple_expr;
            unary = 1;

            yel_parse_expression(yel_tokens);

            --simple_expr;

            unary = 0;
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_or) {
                ++simple_expr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --simple_expr;
            }
        }
        
        // tok_name
        else if(curtype == tok_name) {
            if (nexttype == tok_op_lpar) {
                size_t tmp_i = yel_tokens->pointer;

                ++yel_tokens->pointer;
                ++simple_expr;

                yel_parse_expression(yel_tokens);

                --simple_expr;

                printf("call %s\n", yel_tokens->value[tmp_i]);
            }
            else if (nexttype >= tok_binary_op_div_assign && nexttype <= tok_binary_op_or_assign) {
                printf("push %s\n", cur);

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = nexttype;

                yel_tokens->pointer += 2;
                ++simple_expr;

                yel_parse_expression(yel_tokens);

                --simple_expr;

                switch (tmp_type) {
                case tok_binary_op_div_assign:      puts("div"); break;
                case tok_binary_op_mul_assign:      puts("mul"); break;
                case tok_binary_op_percent_assign:  puts("mod"); break;
                case tok_binary_op_plus_assign:     puts("add"); break;
                case tok_binary_op_minus_assign:    puts("sub"); break;
                case tok_binary_op_rsh_assign:      puts("rsh"); break;
                case tok_binary_op_lsh_assign:      puts("lsh"); break;
                case tok_binary_op_and_assign:      puts("and"); break;
                case tok_binary_op_or_assign:       puts("or"); break;
                }
                
                puts("dup");
                printf("store %s\n", yel_tokens->value[tmp_i]);
            }
            else {
                printf("push %s\n", cur);
            }

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
        if (curtype == tok_name) {
            if (nexttype == tok_binary_op_assign) {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens);

                printf("store (Auto) %s\n", yel_tokens->value[tmp_i]);
            }
            else if (nexttype >= tok_binary_op_div_assign && nexttype <= tok_binary_op_or_assign) {
                printf("push %s\n", cur);

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = nexttype;

                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens);

                switch (tmp_type) {
                case tok_binary_op_div_assign:      puts("div"); break;
                case tok_binary_op_mul_assign:      puts("mul"); break;
                case tok_binary_op_percent_assign:  puts("mod"); break;
                case tok_binary_op_plus_assign:     puts("add"); break;
                case tok_binary_op_minus_assign:    puts("sub"); break;
                case tok_binary_op_rsh_assign:      puts("rsh"); break;
                case tok_binary_op_lsh_assign:      puts("lsh"); break;
                case tok_binary_op_and_assign:      puts("and"); break;
                case tok_binary_op_or_assign:       puts("or"); break;
                }

                printf("store %s\n", yel_tokens->value[tmp_i]);
            }
            else if (nexttype == tok_colon) {
                size_t tmp_i = yel_tokens->pointer;
                size_t tmp_i2 = yel_tokens->pointer += 2;
                ++yel_tokens->pointer;

                yel_parse_expression(yel_tokens);

                printf("store "); 

                switch (yel_tokens->type[tmp_i2])
                {
                case tok_word_Int: printf("(Int) ");break;
                case tok_word_Flt: printf("(Flt) ");break;
                case tok_word_Str: printf("(Str) ");break;
                case tok_word_Any: printf("(Any) ");break;
                default: printf("utype(%s) ", yel_tokens->value[tmp_i2]);break;
                }
                
                puts(yel_tokens->value[tmp_i]);
            }
        }
        else if (curtype == tok_word_return) {
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens);
            printf("ret\n");
        }
        else if (curtype == tok_word_defer) {       // NO REALIZED
            puts("defer:");
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens);
            printf("ret\n");
        }
        else if (curtype == tok_word_break) {
            if (nexttype != tok_semicolon) {
                printf(
                    "Syntax Error: module %s\n--> %lu:%lu: expected ';' \n|\n|", 
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );

                yel_tokens->error = 1;
                return;
            }

            puts("brk");
        }

        ++yel_tokens->pointer;
    }
}
