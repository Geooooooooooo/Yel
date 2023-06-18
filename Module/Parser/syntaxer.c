#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"

#define curtype yel_tokens->type[yel_tokens->pointer]
#define nexttype yel_tokens->type[yel_tokens->pointer+1]
#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]

#define print_ystack() for(int i=0;i<size_t_stack;i++)printf("%d\n",t_stack[i]);

YelTokenType t_stack[64];
int size_t_stack = 0;
_Bool gUnary = 1;
int pars = 0;
int sp = 0;

_Bool yel_check_expr_grammar(YelTokens* yel_tokens) {
_start_:
    while (sp < size_t_stack) {
        if (t_stack[sp] == tok_en_expr && t_stack[sp+1] == tok_binary_op && t_stack[sp+2] == tok_en_expr) {
            register int tmp_sp = sp+1;
            size_t_stack -= 2;

            while (tmp_sp < size_t_stack) {
                t_stack[tmp_sp] = t_stack[tmp_sp+2];
                ++tmp_sp;
            }

            sp = 0;
            continue;
        }
        else if (t_stack[sp] == tok_en_expr && t_stack[sp+1] == tok_comma && t_stack[sp+2] == tok_en_expr) {
            register int tmp_sp = sp+1;
            size_t_stack -= 2;

            while (tmp_sp < size_t_stack) {
                t_stack[tmp_sp] = t_stack[tmp_sp+2];
                ++tmp_sp;
            }

            sp = 0;
            continue;
        }
        else if (t_stack[sp] == tok_op_lpar) {
            if (t_stack[sp+1] == tok_en_expr && t_stack[sp+2] == tok_op_rpar ) {
                t_stack[sp] = tok_en_expr;
                register int tmp_sp = sp+1;
                size_t_stack -= 2;

                while (tmp_sp < size_t_stack) {
                    t_stack[tmp_sp] = t_stack[tmp_sp+2];
                    ++tmp_sp;
                }
            }
            else if (t_stack[sp+1] == tok_op_rpar) {
                t_stack[sp] = tok_en_expr;
                register int tmp_sp = sp+1;
                --size_t_stack;

                while (tmp_sp < size_t_stack) {
                    t_stack[tmp_sp] = t_stack[tmp_sp+1];
                    ++tmp_sp;
                }
            }
            else {
                // INVALID SYNTAX
            }

            sp = 0;
            continue;
        }
        else if (t_stack[sp] == tok_unary_op) {
            if (t_stack[sp+1] == tok_en_expr) {
                t_stack[sp] = tok_en_expr;
                register int tmp_sp = sp+1;
                --size_t_stack;

                while (tmp_sp < size_t_stack) {
                    t_stack[tmp_sp] = t_stack[tmp_sp+1];
                    ++tmp_sp;
                }

                sp = 0;
                continue;
            }
        }
        else if (t_stack[sp] == tok_name && t_stack[sp+1] == tok_op_lpar) {
            if (t_stack[sp+2] == tok_en_expr && t_stack[sp+3] == tok_op_rpar) {
                t_stack[sp] = tok_en_expr;

                register int tmp_sp = sp+1;
                size_t_stack -= 3;

                while (tmp_sp < size_t_stack) {
                    t_stack[tmp_sp] = t_stack[tmp_sp+3];
                    ++tmp_sp;
                } 
                sp = 0;
            }
            else if (t_stack[sp+2] == tok_op_rpar) {
                t_stack[sp] = tok_en_expr;

                register int tmp_sp = sp+1;
                size_t_stack -= 2;

                while (tmp_sp < size_t_stack) {
                    t_stack[tmp_sp] = t_stack[tmp_sp+2];
                    ++tmp_sp;
                } 
                sp = 0;
            }
            else {
                sp += 2;
            }

            continue;
        }
        else if (t_stack[sp] == tok_op_rpar) {
            return 1;
        }
            
        ++sp;
    }

    //print_ystack();
    if (size_t_stack > 1) {
        sp = 0;
        goto _start_;
    }

    puts("");
    print_ystack();
    puts("<expr>");
    return 1;
}

_Bool yel_check_expr(YelTokens* yel_tokens) {
   while (yel_tokens->pointer < yel_tokens->length) {
        if (curtype == tok_op_rpar) {
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_log_or || 
            nexttype == tok_comma || nexttype == tok_semicolon || 
            nexttype == tok_op_rpar) {
                if (pars == 0) {
                    printf("Syntax Error: module %s\n--> %lu:%lu: expected '(' \n|\n|",
                        yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                        yel_tokens->start_symbol[yel_tokens->pointer]
                    );
                    yel_print_error(
                        yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                        yel_tokens->start_symbol[yel_tokens->pointer]
                    );

                    return 0;
                }

                --pars;
                t_stack[size_t_stack] = tok_op_rpar;
                ++size_t_stack;
                gUnary = 0;
                return 1;
            }
            else {
                printf("Syntax Error: module %s\n--> %lu:%lu: invalid syntax \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );

                exit(-1);
            }
        }
        else if (curtype == tok_op_lpar) {
            if (nexttype == tok_number_int || nexttype == tok_number_flt || 
            nexttype == tok_bool || nexttype == tok_name || 
            nexttype == tok_binary_op_plus || nexttype == tok_binary_op_minus ||
            nexttype == tok_unary_op_not || nexttype == tok_op_lpar ||
            nexttype == tok_op_rpar) {
                t_stack[size_t_stack] = tok_op_lpar;
                ++size_t_stack;

                ++pars;
                ++yel_tokens->pointer;

                if (!yel_check_expr(yel_tokens)) {
                    // Error exit
                    exit(-1);
                }
            }
            else {
                printf("Syntax Error: module %s\n--> %lu:%lu: expression is expected \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );

                exit(-1);
            }
        }
        else if (curtype == tok_semicolon) {
            if (pars != 0) {
                printf("Syntax Error: module %s\n--> %lu:%lu: expected ')' \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );

                return 0;
            }

            break;
        }
        else if (curtype == tok_name) {    
            if (nexttype == tok_op_lpar) {
                ++yel_tokens->pointer;
                gUnary = 1;

                if (nexttype >= tok_name && nexttype <= tok_bool ||
                nexttype == tok_binary_op_plus || nexttype == tok_binary_op_minus ||
                nexttype == tok_unary_op_not || nexttype == tok_op_lpar ||
                nexttype == tok_op_rpar) {
                    t_stack[size_t_stack] = tok_name;
                    ++size_t_stack;
                    t_stack[size_t_stack] = tok_op_lpar;
                    ++size_t_stack;

                    ++pars;
                    ++yel_tokens->pointer;

                    if (!yel_check_expr(yel_tokens)) {
                        // Error exit
                        exit(-1);
                    }
                }
                else {
                    printf("Syntax Error: module %s\n--> %lu:%lu: expression is expected \n|\n|",
                        yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                        yel_tokens->start_symbol[yel_tokens->pointer+1]
                    );
                    yel_print_error(
                        yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                        yel_tokens->start_symbol[yel_tokens->pointer+1]
                    );

                    exit(-1);
                }
            }
            else if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_assign || 
            nexttype == tok_comma || nexttype == tok_semicolon) {
                t_stack[size_t_stack] = tok_en_expr;
                ++size_t_stack;
            }
            else {
                printf("Syntax Error: module %s\n--> %lu:%lu: operator is expected \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );

                exit(-1);
            }
        }
        else if (curtype == tok_number_int || curtype == tok_number_flt || curtype == tok_bool) {
            if (nexttype >= tok_binary_op_pow && nexttype <= tok_binary_op_assign || 
            nexttype == tok_comma || nexttype == tok_semicolon || nexttype == tok_op_rpar) {
                t_stack[size_t_stack] = tok_en_expr;
                ++size_t_stack;
                gUnary = 0;
            } 
            else {
                printf("Syntax Error: module %s\n--> %lu:%lu: operator is expected \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );

                exit(-1);
            }
        }
        else if (curtype == tok_unary_op_not && gUnary) {
            t_stack[size_t_stack] = tok_unary_op;
            ++size_t_stack;
        }
        else if (curtype >= tok_binary_op_pow && curtype <= tok_binary_op_assign) {
            if (nexttype >= tok_name && nexttype <= tok_bool ||
            nexttype == tok_binary_op_plus || nexttype == tok_binary_op_minus ||
            nexttype == tok_unary_op_not || nexttype == tok_op_lpar) {
                if (gUnary) t_stack[size_t_stack] = tok_unary_op;
                else t_stack[size_t_stack] = tok_binary_op;
                ++size_t_stack;
                gUnary = 1;
            }
            else {
                printf("Syntax Error: module %s\n--> %lu:%lu: expression is expected \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );

                exit(-1);
            }
        }
        else if (curtype == tok_comma) {
             if (nexttype >= tok_name && nexttype <= tok_bool ||
            nexttype == tok_binary_op_plus || nexttype == tok_binary_op_minus ||
            nexttype == tok_unary_op_not || nexttype == tok_op_lpar) {
                t_stack[size_t_stack] = tok_comma;
                ++size_t_stack;
                gUnary = 1;
            }
            else {
                printf("Syntax Error: module %s\n--> %lu:%lu: expression is expected \n|\n|",
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]
                );

                exit(-1);
            }
        }
        else {
            printf("Syntax Error: module %s\n--> %lu:%lu: invalid syntax \n|\n|",
            yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
            yel_tokens->start_symbol[yel_tokens->pointer]
            );
            yel_print_error(
                yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                yel_tokens->start_symbol[yel_tokens->pointer]
            );

            exit(-1);
        }
        
        ++yel_tokens->pointer;
    }

    print_ystack();
    return yel_check_expr_grammar(yel_tokens);
}

YelEntities yel_define_next_entity(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (curtype == tok_name) {
            if (nexttype == tok_binary_op_assign) {

            }
        }

        ++yel_tokens->pointer;
    }
}

void yel_gen_parse_tree(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        printf("%d\n", yel_define_next_entity(yel_tokens));
    }
}