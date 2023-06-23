#include "../Parser/parser.h"
#include "../Parser/syntaxer.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"
#include "../Dependencies/dependencies.h"

#define tmpPRINT_OPCODE() \
    if (_CurType == tok_name)printf("load_fast %s\n",_CurVal);else printf("load_const %s\n",_CurVal);instCounter+=2;++argCounter;

int          _SimpleExpr = 0;
static int   _Parentheses = 0;      // ()
static _Bool _Unary = 1;
static size_t instCounter = 0; 
static size_t argCounter = 0;

// from parser.c
void yel_parse_expression(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_semicolon) {            
            ++yel_tokens->pointer;
            _SimpleExpr = 0;
            return;
        }
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

            ++instCounter;
            if (_SimpleExpr) return;
        }

        // (
        else if (_CurType == tok_op_lpar) {
            ++_Parentheses;
            ++yel_tokens->pointer;
            _Unary = 1;

            register int tmp_SimpleExpr = _SimpleExpr;
            _SimpleExpr = 0;
            yel_parse_expression(yel_tokens);
            _SimpleExpr = tmp_SimpleExpr;

            if (_SimpleExpr) return;
        }
        else if (_CurType == tok_op_rpar) {
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
            ++instCounter;
            --argCounter;

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
            ++instCounter;
            --argCounter;

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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
        }
        
        // +, -
        else if ((_NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus) && _CurType != tok_op_lpar &&  
        _CurType != tok_binary_op_plus && _CurType != tok_binary_op_minus) {
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
                
            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;

            if (_SimpleExpr) return;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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

            ++instCounter;
            --argCounter;
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
            ++instCounter;
            --argCounter;
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
            ++instCounter;
            --argCounter;
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
            ++instCounter;
            --argCounter;
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
            ++instCounter;
            --argCounter;
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
            ++instCounter;
            --argCounter;
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
            ++instCounter;
            --argCounter;
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

                size_t tmp_argCounter = argCounter;
                argCounter = 0;

                yel_parse_expression(yel_tokens);

                --_SimpleExpr;
                instCounter += 4;

                printf("load_const %lu          ; args\n", argCounter);
                printf("load_const %lu          ; return address\n", instCounter+3);
                printf("call %s\n", yel_tokens->value[tmp_i]);

                instCounter += 2;
                argCounter = tmp_argCounter + 1;
                _Unary = 0;
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                if (_NextType != tok_binary_op_assign) 
                    tmpPRINT_OPCODE();

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = _NextType;

                ++_SimpleExpr;
                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens);

                --_SimpleExpr;

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
                printf("store %s\n", yel_tokens->value[tmp_i]);

                instCounter += 4;
                --argCounter;
            }
            else {
                printf("load_fast %s\n", _CurVal);
                instCounter += 2;
                ++argCounter;
                _Unary = 0;
            }

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_number_int || _CurType == tok_number_flt) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                yel_print_error("SyntaxError", "expression on the left should not be a constant", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                yel_tokens->error = 1;
            }

            printf("load_const %s\n", _CurVal);
            instCounter += 2;
            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_string) {
            printf("load_const '%s'\n", _CurVal);
            instCounter += 2;
            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_bool) {
            printf("load_const %s\n", _CurVal);
            instCounter += 2;
            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        ++yel_tokens->pointer;
        if (_NextType == tok_semicolon) return;
    }
}

// from parser.c
void yel_parse_statement(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_name) {
            if (_NextType == tok_binary_op_assign) {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens);

                printf("store %s\n", yel_tokens->value[tmp_i]);
                instCounter += 2;
                return;
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_pow_assign) {
                printf("load_fast %s\n", _CurVal);

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

                printf("store %s\n", yel_tokens->value[tmp_i]);

                instCounter += 5;
            }
        }
        else if (_CurType == tok_word_return) {
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens);
            printf("ret\n");
            ++instCounter;
        }
        else if (_CurType == tok_word_defer) {       // NO REALIZED
            puts("defer:");
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens);
            printf("ret\n");
        }
        else if (_CurType == tok_word_break) {
            puts("brk");
            ++instCounter;
        }
        else if (_CurType == tok_word_if) {
            ++yel_tokens->pointer;
            if (yel_check_expr(yel_tokens, 1) == RET_CODE_OK) {
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            printf("jump_zero [addr]\n\n");

            instCounter += 2; //

            ++yel_tokens->pointer;

            if (_CurType != tok_op_flbrk) {
                ++yel_tokens->pointer;
                if (_CurType == tok_name &&(_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                _CurType == tok_word_break || 
                _CurType == tok_word_return || _CurType == tok_op_flbrk || _CurType == tok_op_frbrk || _CurType == tok_word_if) {
                    if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                        yel_parse_statement(yel_tokens);
                    } else break;
                }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                    yel_parse_expression(yel_tokens);
                } else break;
            }
            else if (_CurType == tok_op_flbrk) {
                int fbrks = 0;

                while (yel_tokens->pointer < yel_tokens->length) {
                    if (_CurType == tok_op_flbrk) {
                        ++yel_tokens->pointer;
                        ++fbrks;
                    }  else if (_CurType == tok_op_frbrk) {
                        ++yel_tokens->pointer;
                        --fbrks;
                    } if (fbrks == 0) break;
                    
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break || 
                    _CurType == tok_word_return || _CurType == tok_op_flbrk || _CurType == tok_op_frbrk || _CurType == tok_word_if) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens);
                    } else break;
                }
            }

            puts("end_if:");

            if (_CurType == tok_word_else) {
                puts("jump [end_if]\n\nelse:");
                instCounter += 2;

                ++yel_tokens->pointer;

                if (_CurType != tok_op_flbrk) {
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                    _CurType == tok_op_frbrk || _CurType == tok_word_if) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens);
                        } else break;
                    }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens);
                    } else break;
                }
                else if (_CurType == tok_op_flbrk) {
                    int fbrks = 0;

                    while (yel_tokens->pointer < yel_tokens->length) {
                        if (_CurType == tok_op_flbrk) {
                            ++yel_tokens->pointer;
                            ++fbrks;
                        }  else if (_CurType == tok_op_frbrk) {
                            ++yel_tokens->pointer;
                            --fbrks;
                        } if (fbrks == 0) break;
                        
                        if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                        _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                        _CurType == tok_op_frbrk || _CurType == tok_word_if) {
                            if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                                yel_parse_statement(yel_tokens);
                            } else break;
                        } 
                        else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                            yel_parse_expression(yel_tokens);
                        } else break;
                    }
                }

                printf("\nend_if:");
            }

            return;
        }
        else if (_CurType == tok_word_while) {
            int tmp_inst = instCounter+1;

            ++yel_tokens->pointer;
            if (yel_check_expr(yel_tokens, 1) == RET_CODE_OK) {
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens);
                --_SimpleExpr;
            }

            printf("jump_zero [end_while]\npop\n\n");
            instCounter += 2;

            ++yel_tokens->pointer;

            if (_CurType != tok_op_flbrk) {
                ++yel_tokens->pointer;
                if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                _CurType == tok_op_frbrk || _CurType == tok_word_if) {
                    if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                        yel_parse_statement(yel_tokens);
                    } else break;
                }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                    yel_parse_expression(yel_tokens);
                } else break;
            }
            else if (_CurType == tok_op_flbrk) {
                int fbrks = 0;

                while (yel_tokens->pointer < yel_tokens->length) {
                    if (_CurType == tok_op_flbrk) {
                        ++yel_tokens->pointer;
                        ++fbrks;
                    }  else if (_CurType == tok_op_frbrk) {
                        ++yel_tokens->pointer;
                        --fbrks;
                    } if (fbrks == 0) break;
                    
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                    _CurType == tok_op_frbrk || _CurType == tok_word_if) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens);
                    } else break;
                }
            }

            printf("jump %d\nend_while:\npop\n", tmp_inst);
        }

        ++yel_tokens->pointer;
    }
}
