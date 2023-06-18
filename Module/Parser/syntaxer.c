#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"

#define curtype yel_tokens->type[yel_tokens->pointer]
#define nexttype yel_tokens->type[yel_tokens->pointer+1]
#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]

YelTokenType t_stack[64];
int size_t_stack = 0;
_Bool gUnary = 1;
int pars = 0;
int sp = 0;

_Bool yel_check_expr_grammar(YelTokens* yel_tokens) {
    while (sp < size_t_stack) {
        if (t_stack[sp] == tok_en_expr && t_stack[sp+1] == tok_binary_op && t_stack[sp+2] == tok_en_expr) {
            register int tmp_sp = sp+1;
            size_t_stack -= 2;

            while (tmp_sp < size_t_stack) {
                t_stack[tmp_sp] = t_stack[tmp_sp+2];
                ++tmp_sp;
            }

            continue;
        }
        else if (t_stack[sp] == tok_en_expr && t_stack[sp+1] == tok_comma && t_stack[sp+2] == tok_en_expr) {
            register int tmp_sp = sp+1;
            size_t_stack -= 2;

            while (tmp_sp < size_t_stack) {
                t_stack[tmp_sp] = t_stack[tmp_sp+2];
                ++tmp_sp;
            }

            continue;
        }
        else if (t_stack[sp] == tok_name && t_stack[sp+1] == tok_op_lpar) {
            if (t_stack[sp+2] == tok_en_expr && t_stack[sp+3] == tok_op_rpar || t_stack[sp+2] == tok_op_rpar) {
                t_stack[sp] = tok_en_expr;

                register int tmp_sp = sp+1;
                size_t_stack -= 3;

                while (tmp_sp < size_t_stack) {
                    t_stack[tmp_sp] = t_stack[tmp_sp+3];
                    ++tmp_sp;
                }

                continue;
            }
            else {
                int tmp_sp = sp;
                sp += 2;

                yel_check_expr_grammar(yel_tokens);

                sp = tmp_sp;
                continue;
            }
        }
        else if (t_stack[sp] == tok_op_rpar) {
            return 1;
        }
            
        ++sp;
    }

    printf("%d\n", t_stack[0]);
    return 1;
}

_Bool yel_check_expr(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (curtype == tok_op_rpar) {
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
        }
        else if (curtype == tok_semicolon){
            break;
        }
        else if (curtype == tok_name) {    
            if (nexttype == tok_op_lpar) {
                t_stack[size_t_stack] = tok_name;
                ++size_t_stack;
                t_stack[size_t_stack] = tok_op_lpar;
                ++size_t_stack;

                ++pars;
                yel_tokens->pointer += 2;

                if (!yel_check_expr(yel_tokens)) {
                    // Error exit
                    exit(-1);
                }
            }
            else if (nexttype != tok_op_lpar) {
                t_stack[size_t_stack] = tok_en_expr;
                ++size_t_stack;
            }
            else {
                printf("else => nexttype = %d\n", nexttype);
            }
        }
        else if (curtype == tok_number_int || curtype == tok_number_flt) {
            t_stack[size_t_stack] = tok_en_expr;
            ++size_t_stack;
        }
        else if (curtype >= tok_binary_op_pow && curtype <= tok_binary_op_assign) {
            t_stack[size_t_stack] = tok_binary_op;
            ++size_t_stack;
        }
        else if (curtype == tok_comma) {
            t_stack[size_t_stack] = tok_comma;
            ++size_t_stack;
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