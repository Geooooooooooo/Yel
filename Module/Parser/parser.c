#include "../Parser/parser.h"
#include "../Parser/syntaxer.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"
#include "../Dependencies/dependencies.h"

#define tmpPRINT_OPCODE() \
    if (_CurType == tok_number_int)\
        printf("push_const (Int) %s\n", _CurVal);\
    else if (_CurType == tok_number_flt)\
        printf("push_const (Flt) %s\n", _CurVal);\
    else if (_CurType == tok_string)\
        printf("push_const (Str) '%s'\n", _CurVal);\
     else if (_CurType == tok_bool)\
        printf("push_const (Bool) %s\n", _CurVal);\
    else printf("load_fast %s\n", _CurVal);

int          _SimpleExpr = 0;
static int   _Parentheses = 0;
static _Bool _Unary = 1;

void yel_parse_expression(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_semicolon) {
            if (_Parentheses) {
                printf(
                    "Syntax Error: module %s\n--> %lu:%lu: expected ')' \n|\n|", 
                    yel_tokens->file_name, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer-1]
                );
                yel_print_error(
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer-1]
                );

                yel_tokens->error = 1;
            }

            ++yel_tokens->pointer;

            return;
        }

        // UNARY OPERATOR
        else if (_Unary && (
            _CurType == tok_binary_op_plus || _CurType == tok_binary_op_minus ||
            (_CurType >= tok_unary_op_not && _CurType <= tok_unary_op_dec)
        )) {
            size_t uo_ptr = yel_tokens->pointer;
            ++_SimpleExpr;
            ++yel_tokens->pointer;

            yel_parse_expression(yel_tokens);

            _Unary = 0;
            --_SimpleExpr;

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

            if (_SimpleExpr) return;
        }

        // (
        else if (_CurType == tok_op_lpar) {
            if (_NextType == tok_op_rpar) {
                puts("load_fast None");
            }

            ++_Parentheses;
            ++yel_tokens->pointer;
            _Unary = 1;

            register int tmp__SimpleExpr = _SimpleExpr;
            _SimpleExpr = 0;
            yel_parse_expression(yel_tokens);
            _SimpleExpr = tmp__SimpleExpr;

            if (_SimpleExpr) return;
        }
        else if (_CurType == tok_op_rpar) {
            if (_Parentheses == 0) {
                printf(
                    "Syntax Error: module %s\n--> %lu:%lu: expected '(' \n|\n|", 
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

            --_Parentheses;
            _Unary = 0;

            return;
        }

        // **
        else if (_NextType == tok_binary_op_pow) {
            tmpPRINT_OPCODE();

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;
                
            yel_parse_expression(yel_tokens);
                
            --_SimpleExpr;

            _Unary = 0;
            puts("pow");
            if (_SimpleExpr) return;
        }
        else if (_CurType == tok_binary_op_pow) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            puts("pow");
            if (_SimpleExpr) return;
        }

        // *, /, %
        else if(_NextType == tok_binary_op_div || _NextType == tok_binary_op_mul || _NextType == tok_binary_op_percent) {
            tmpPRINT_OPCODE();
            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;
            
            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            if (_NextType == tok_binary_op_pow) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            // Release version
            // op-code = _CurType
            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                puts("div");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                puts("mul");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                puts("mod");
        }
        else if(_CurType == tok_binary_op_div || _CurType == tok_binary_op_mul || _CurType == tok_binary_op_percent) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;
            
            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            if (_NextType == tok_binary_op_pow) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            // Release version
            // op-code = _CurType
            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                puts("div");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                puts("mul");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                puts("mod");

        }
        
        // +, -
        else if (_NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_percent) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                puts("add");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                puts("sub");

        }
        else if (_CurType == tok_binary_op_plus || _CurType == tok_binary_op_minus) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_percent) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                puts("add");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                puts("sub");

            if (_SimpleExpr) return;
            continue; 
        }
        
        // <<, >>
        else if (_NextType == tok_binary_op_rsh || _NextType == tok_binary_op_lsh) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_minus) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                puts("rsh");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                puts("lsh");
        }
        else if (_CurType == tok_binary_op_rsh || _CurType == tok_binary_op_lsh) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_minus) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                puts("rsh");
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                puts("lsh");
        }

        // <, <=, >, >=
        else if (_NextType >= tok_binary_op_more && _NextType <= tok_binary_op_less_eq) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_lsh) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
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
        else if (_CurType >= tok_binary_op_more && _CurType <= tok_binary_op_less_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_lsh) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
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
        else if (_NextType == tok_binary_op_eq || _NextType == tok_binary_op_not_eq) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_less_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                puts("eq");
            else puts("not_eq");
        }
        else if (_CurType == tok_binary_op_eq || _CurType == tok_binary_op_not_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_less_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                puts("eq");
            else puts("not_eq");
        }

        // &
        else if (_NextType == tok_binary_op_log_and) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);
            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_not_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            puts("and");
        }
        else if (_CurType == tok_binary_op_log_and) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_not_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }
            
            puts("and");
        }
        
        // |
        else if (_NextType == tok_binary_op_log_or) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            puts("or");
        }
        else if (_CurType == tok_binary_op_log_or) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }
            
            puts("or");
        }

        // &&
        else if (_NextType == tok_binary_op_and) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            puts("log_and");
        }
        else if (_CurType == tok_binary_op_and) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }
            
            puts("log_and");
        }
        
        // ||
        else if (_NextType == tok_binary_op_or) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            puts("log_or");
        }
        else if (_CurType == tok_binary_op_or) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }
            
            puts("log_or");
        }

        // ,
        else if (_NextType == tok_comma) {
            tmpPRINT_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }
        }
        else if (_CurType == tok_comma) {
            if (_SimpleExpr) return;

            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }
        }
        
        // tok_name
        else if(_CurType == tok_name) {
            if (_NextType == tok_op_lpar) {
                size_t tmp_i = yel_tokens->pointer;

                ++yel_tokens->pointer;
                ++_SimpleExpr;

                yel_parse_expression(yel_tokens);

                --_SimpleExpr;

                printf("call %s\n", yel_tokens->value[tmp_i]);
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                if (_NextType != tok_binary_op_assign) 
                    tmpPRINT_OPCODE();

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = _NextType;

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
                case tok_binary_op_pow_assign:      puts("pow");break;
                case tok_binary_op_assign:break;
                }
                
                puts("dup");
                printf("store (Auto) %s\n", yel_tokens->value[tmp_i]);
            }
            else {
                printf("load_fast %s\n", _CurVal);
                _Unary = 0;
            }

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_number_int || _CurType == tok_number_flt) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                printf(
                    "Syntax Error: module %s\n--> %lu:%lu: expression on the left should not be a constant \n|\n|", 
                    yel_tokens->file_name, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );
                yel_print_error(
                    yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]
                );

                exit(-1);
            }

            if (_CurType == tok_number_int)
                printf("push_const (Int) %s\n", _CurVal);
            else if (_CurType == tok_number_flt)
                printf("push_const (Flt) %s\n", _CurVal);

            _Unary = 0;
            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_string) {
            printf("push_const (Str) '%s'\n", _CurVal);
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_bool) {
            printf("push_const (Bool) %s\n", _CurVal);
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        ++yel_tokens->pointer;
    }
}

void yel_parse_statement(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_name) {
            if (_NextType == tok_binary_op_assign) {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens);

                printf("store (Auto) %s\n", yel_tokens->value[tmp_i]);
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_pow_assign) {
                if (_CurType == tok_number_int)
                    printf("push_const (Int) %s\n", _CurVal);
                else if (_CurType == tok_number_flt)
                    printf("push_const (Flt) %s\n", _CurVal);
                else if (_CurType == tok_word_Bool)
                    printf("push_const (Bool) %s\n", _CurVal);
                else printf("load_fast (Auto) %s\n", _CurVal);

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = _NextType;

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
                case tok_binary_op_pow_assign:      puts("pow");break;
                }

                printf("store (Auto) %s\n", yel_tokens->value[tmp_i]);
            }
            else if (_NextType == tok_colon) {
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
                case tok_word_Bool: printf("(Bool) ");break;
                default: printf("[%s] ", yel_tokens->value[tmp_i2]);break;
                }
                
                puts(yel_tokens->value[tmp_i]);
            }
        }
        else if (_CurType == tok_word_return) {
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens);
            printf("ret\n");
        }
        else if (_CurType == tok_word_defer) {       // NO REALIZED
            puts("defer:");
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens);
            printf("ret\n");
        }
        else if (_CurType == tok_word_break) {
            puts("brk");
        }

        ++yel_tokens->pointer;
    }
}