#include "../Parser/parser.h"
#include "../Parser/syntaxer.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"
#include "../Dependencies/dependencies.h"

#define tmpPRINT_OPCODE() {\
    if (_CurType == tok_name) {opcodes->codes[opcodes->len] = LOAD_VALUE;\
        opcodes->codes[opcodes->len+1] = (OPCODEWORD)_CurVal;}\
    else {opcodes->codes[opcodes->len] = LOAD_CONST;\
        opcodes->codes[opcodes->len+1] = &_CurVal;}opcodes->len+=2;++argCounter;}

int          _SimpleExpr = 0;
static int   _Parentheses = 0;      // ()
static _Bool _Unary = 1;
static size_t argCounter = 0;
static size_t curInstLen = 0;

// from parser.c
void yel_parse_expression(YelTokens* yel_tokens, OPCODES* opcodes) {
    if (curInstLen < opcodes->len+5) {
        curInstLen = opcodes->len + 20;
        opcodes->codes = realloc(opcodes->codes, (curInstLen)*sizeof(OPCODEWORD));
    }

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

            yel_parse_expression(yel_tokens, opcodes);

            _Unary = 0;
            --_SimpleExpr;

            if (yel_tokens->type[uo_ptr] == tok_unary_op_pos)
                opcodes->codes[opcodes->len] = UNARY_POS;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_neg)
                opcodes->codes[opcodes->len] = UNARY_NEG;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_not)
                opcodes->codes[opcodes->len] = UNARY_NOT;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_inc)
                opcodes->codes[opcodes->len] = UNARY_INC;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_dec)
                opcodes->codes[opcodes->len] = UNARY_DEC;

            ++opcodes->len;
            if (_SimpleExpr) return;
        }

        // (
        else if (_CurType == tok_op_lpar) {
            ++_Parentheses;
            ++yel_tokens->pointer;
            _Unary = 1;

            register int tmp_SimpleExpr = _SimpleExpr;
            _SimpleExpr = 0;
            yel_parse_expression(yel_tokens, opcodes);
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
                
            yel_parse_expression(yel_tokens, opcodes);
                
            --_SimpleExpr;
            _Unary = 0;

            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_POW;

            opcodes->len += 2;
            --argCounter;

            if (_SimpleExpr) return;
        }
        else if (_CurType == tok_binary_op_pow) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;
            _Unary = 0;

            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_POW;
            opcodes->len += 2;

            ++opcodes->len;
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
            
            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            if (_NextType == tok_binary_op_pow) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                opcodes->codes[opcodes->len+1] = BYNARY_DIV;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                opcodes->codes[opcodes->len+1] = BYNARY_MUL;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                opcodes->codes[opcodes->len+1] = BYNARY_MOD;

            opcodes->len += 2;
            --argCounter;
        }
        else if(_CurType == tok_binary_op_div || _CurType == tok_binary_op_mul || _CurType == tok_binary_op_percent) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;
            
            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            if (_NextType == tok_binary_op_pow) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                opcodes->codes[opcodes->len+1] = BYNARY_DIV;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                opcodes->codes[opcodes->len+1] = BYNARY_MUL;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                opcodes->codes[opcodes->len+1] = BYNARY_MOD;

            opcodes->len += 2;
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

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_percent) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                opcodes->codes[opcodes->len+1] = BYNARY_ADD;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                opcodes->codes[opcodes->len+1] = BYNARY_SUB;
                
            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_plus || _CurType == tok_binary_op_minus) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_percent) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                opcodes->codes[opcodes->len+1] = BYNARY_ADD;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                opcodes->codes[opcodes->len+1] = BYNARY_SUB;
                
            opcodes->len += 2;
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

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_minus) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                opcodes->codes[opcodes->len+1] = BYNARY_RSH;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                opcodes->codes[opcodes->len+1] = BYNARY_LSH;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_rsh || _CurType == tok_binary_op_lsh) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_minus) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                opcodes->codes[opcodes->len+1] = BYNARY_RSH;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                opcodes->codes[opcodes->len+1] = BYNARY_LSH;

            opcodes->len += 2;
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

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_lsh) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_more) 
                opcodes->codes[opcodes->len+1] = BYNARY_MORE;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less) 
                opcodes->codes[opcodes->len+1] = BYNARY_LESS;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_more_eq)
                opcodes->codes[opcodes->len+1] = BYNARY_MORE_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less_eq)
                opcodes->codes[opcodes->len+1] = BYNARY_LESS_EQ;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType >= tok_binary_op_more && _CurType <= tok_binary_op_less_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_lsh) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_more) 
                opcodes->codes[opcodes->len+1] = BYNARY_MORE;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less) 
                opcodes->codes[opcodes->len+1] = BYNARY_LESS;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_more_eq)
                opcodes->codes[opcodes->len+1] = BYNARY_MORE_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less_eq)
                opcodes->codes[opcodes->len+1] = BYNARY_LESS_EQ;

            opcodes->len += 2;
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

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_less_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                 opcodes->codes[opcodes->len+1] = BYNARY_EQ;
            else 
                opcodes->codes[opcodes->len+1] = BYNARY_NOT_EQ;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_eq || _CurType == tok_binary_op_not_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_less_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                 opcodes->codes[opcodes->len+1] = BYNARY_EQ;
            else 
                opcodes->codes[opcodes->len+1] = BYNARY_NOT_EQ;

            opcodes->len += 2;
            --argCounter;
        }

        // &
        else if (_NextType == tok_binary_op_log_and) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);
            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_not_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_AND;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_log_and) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_not_eq) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }
            
            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_AND;

            opcodes->len += 2;
            --argCounter;
        }
        
        // |
        else if (_NextType == tok_binary_op_log_or) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_OR;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_log_or) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }
            
            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_OR;

            opcodes->len += 2;
            --argCounter;
        }

        // &&
        else if (_NextType == tok_binary_op_and) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_LOGICAL_AND;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_and) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }
            
            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_LOGICAL_AND;

            opcodes->len += 2;
            --argCounter;
        }
        
        // ||
        else if (_NextType == tok_binary_op_or) {
            tmpPRINT_OPCODE();

            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            
            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_LOGICAL_OR;

            opcodes->len += 2;
            --argCounter;
        }
        else if (_CurType == tok_binary_op_or) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }
            
            
            opcodes->codes[opcodes->len] = BYNARY_OP;
            opcodes->codes[opcodes->len+1] = BYNARY_LOGICAL_OR;

            opcodes->len += 2;
            --argCounter;
        }

        // ,
        else if (_NextType == tok_comma) {
            tmpPRINT_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }
        }
        else if (_CurType == tok_comma) {
            if (_SimpleExpr) return;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, opcodes);

            --_SimpleExpr;

            _Unary = 0;
            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, opcodes);
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

                yel_parse_expression(yel_tokens, opcodes);

                --_SimpleExpr;

                //printf("load_const %lu           ; args\n", argCounter);
                //printf("load_const %lu          ; return address\n", opcodes->len+7);
                //printf("call %s\n", yel_tokens->value[tmp_i]);

                opcodes->codes[opcodes->len] = LOAD_CONST;
                opcodes->codes[opcodes->len+1] = &argCounter;         // addres to constant
                opcodes->codes[opcodes->len+2] = OP_CALL;
                opcodes->codes[opcodes->len+3] = yel_tokens->value[tmp_i];

                opcodes->len += 4;
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

                yel_parse_expression(yel_tokens, opcodes);

                --_SimpleExpr;

                switch (tmp_type) {
                case tok_binary_op_div_assign:      opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_DIV; break;
                case tok_binary_op_mul_assign:      opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_MUL; break;
                case tok_binary_op_percent_assign:  opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_MOD; break;
                case tok_binary_op_plus_assign:     opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_ADD; break;
                case tok_binary_op_minus_assign:    opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_SUB; break;
                case tok_binary_op_rsh_assign:      opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_RSH; break;
                case tok_binary_op_lsh_assign:      opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_LSH; break;
                case tok_binary_op_and_assign:     opcodes->codes[opcodes->len] = BYNARY_OP; opcodes->codes[opcodes->len+1] = BYNARY_AND; break;
                case tok_binary_op_or_assign:       opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_OR;  break;
                case tok_binary_op_pow_assign:      opcodes->codes[opcodes->len] = BYNARY_OP;opcodes->codes[opcodes->len+1] = BYNARY_POW; break;
                case tok_binary_op_assign:          opcodes->len -= 2;                      break;
                }
                
                opcodes->codes[opcodes->len+2] = DUP_VALUE;
                opcodes->codes[opcodes->len+3] = OP_STORE;
                opcodes->codes[opcodes->len+4] = yel_tokens->value[tmp_i];

                opcodes->len += 5;
                --argCounter;
            }
            else {
                
                opcodes->codes[opcodes->len] = LOAD_VALUE;
                opcodes->codes[opcodes->len+1] = _CurVal;
                opcodes->len += 2;

                ++argCounter;
                _Unary = 0;
            }

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_number_int || _CurType == tok_number_flt || _CurType == tok_string || _CurType == tok_bool) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                yel_print_error("SyntaxError", "expression on the left should not be a constant", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                yel_tokens->error = 1;
            }

            
            opcodes->codes[opcodes->len] = LOAD_CONST;
            opcodes->codes[opcodes->len+1] = &_CurVal;            // add to const
            opcodes->len += 2;

            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        ++yel_tokens->pointer;
        if (_NextType == tok_semicolon) return;
    }
}

// from parser.c
void yel_parse_statement(YelTokens* yel_tokens, OPCODES* opcodes) {
    if (curInstLen < opcodes->len+5) {
        curInstLen = opcodes->len + 40;
        opcodes->codes = realloc(opcodes->codes, (curInstLen)*sizeof(OPCODEWORD));
    }

    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_semicolon) {
            ++yel_tokens->pointer;
            break;
        }
        else if (_CurType == tok_name) {
            if (_NextType == tok_binary_op_assign) {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens, opcodes);
                
                opcodes->codes[opcodes->len] = OP_STORE;
                opcodes->codes[opcodes->len+1] = yel_tokens->value[tmp_i];
                opcodes->len += 2;

                return;
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_pow_assign) {
                
                opcodes->codes[opcodes->len] = LOAD_VALUE;
                opcodes->codes[opcodes->len+1] = _CurVal;
                opcodes->len += 2;

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = _NextType;

                yel_tokens->pointer += 2;

                _Unary = 1;
                yel_parse_expression(yel_tokens, opcodes);
                _Unary = 0;

                opcodes->codes[opcodes->len] = BYNARY_OP;
                switch (tmp_type) {
                case tok_binary_op_div_assign:      opcodes->codes[opcodes->len+1] = BYNARY_DIV; break;
                case tok_binary_op_mul_assign:      opcodes->codes[opcodes->len+1] = BYNARY_MUL; break;
                case tok_binary_op_percent_assign:  opcodes->codes[opcodes->len+1] = BYNARY_MOD; break;
                case tok_binary_op_plus_assign:     opcodes->codes[opcodes->len+1] = BYNARY_ADD; break;
                case tok_binary_op_minus_assign:    opcodes->codes[opcodes->len+1] = BYNARY_SUB; break;
                case tok_binary_op_rsh_assign:      opcodes->codes[opcodes->len+1] = BYNARY_RSH; break;
                case tok_binary_op_lsh_assign:      opcodes->codes[opcodes->len+1] = BYNARY_LSH; break;
                case tok_binary_op_and_assign:      opcodes->codes[opcodes->len+1] = BYNARY_AND; break;
                case tok_binary_op_or_assign:       opcodes->codes[opcodes->len+1] = BYNARY_OR;  break;
                case tok_binary_op_pow_assign:      opcodes->codes[opcodes->len+1] = BYNARY_POW; break;
                }

                opcodes->codes[opcodes->len+2] = OP_STORE;
                opcodes->codes[opcodes->len+3] = yel_tokens->value[tmp_i];

                opcodes->len += 4;
                return;
            }
        }
        else if (_CurType == tok_word_return) {
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens, opcodes);

            opcodes->codes[opcodes->len] = OP_RET;
            ++opcodes->len;
            return;
        }
        else if (_CurType == tok_word_defer) {       // NO REALIZED
            puts("defer:");
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens, opcodes);
            printf("ret\n");
            return;
        }
        else if (_CurType == tok_word_break) {
            opcodes->codes[opcodes->len] = OP_JUMP_TO;
            opcodes->codes[opcodes->len+1] = OP_BRK;

            opcodes->len += 2;

            yel_tokens->pointer += 2;
            return;
        }
        else if (_CurType == tok_word_continue) {
            opcodes->codes[opcodes->len] = OP_JUMP_TO;
            opcodes->codes[opcodes->len+1] = OP_CNT;

            opcodes->len += 2;

            yel_tokens->pointer += 2;
            return;
        }
        else if (_CurType == tok_word_if) {
            ++yel_tokens->pointer;
            if (yel_check_expr(yel_tokens, 1) == RET_CODE_OK) {
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }
            
            opcodes->codes[opcodes->len] = OP_JUMP_ZERO;
            size_t _NextAddress = opcodes->len+1;

            opcodes->len += 2;

            ++yel_tokens->pointer;

            if (_CurType != tok_op_flbrk) {
                if (_CurType == tok_name &&(_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                _CurType == tok_word_break || 
                _CurType == tok_word_return || _CurType == tok_op_flbrk || _CurType == tok_op_frbrk || 
                _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                    if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                        yel_parse_statement(yel_tokens, opcodes);
                    } else break;
                }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                    yel_parse_expression(yel_tokens, opcodes);
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
                    _CurType == tok_word_return || _CurType == tok_op_flbrk || _CurType == tok_op_frbrk || 
                    _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, opcodes);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, opcodes);
                    } else break;
                }
            }

            if (_CurType == tok_word_else) {
                opcodes->codes[_NextAddress] = opcodes->len+2;
                
                opcodes->codes[opcodes->len] = OP_JUMP_TO;
                _NextAddress = opcodes->len+1;

                opcodes->len += 2;

                ++yel_tokens->pointer;

                if (_CurType != tok_op_flbrk) {
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                    _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, opcodes);
                        } else break;
                    }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, opcodes);
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
                        _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                            if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                                yel_parse_statement(yel_tokens, opcodes);
                            } else break;
                        } 
                        else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                            yel_parse_expression(yel_tokens, opcodes);
                        } else break;
                    }
                }

                opcodes->codes[_NextAddress] = opcodes->len;            
            }
            else {
                opcodes->codes[_NextAddress] = opcodes->len;
            }

            return;
        }
        else if (_CurType == tok_word_while) {
            int tmp_inst = opcodes->len+1;
            ++yel_tokens->pointer;

            size_t while_start = opcodes->len;

            if (yel_check_expr(yel_tokens, 1) == RET_CODE_OK) {
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, opcodes);
                --_SimpleExpr;
            }

            opcodes->codes[opcodes->len] = OP_JUMP_ZERO;
            size_t _NextAddress = opcodes->len+1;
            opcodes->codes[opcodes->len+2] = POP_VALUE;

            opcodes->len += 3;

            //printf("jump_zero [end_while]\npop\n\n");

            ++yel_tokens->pointer;

            if (_CurType != tok_op_flbrk) {
                if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                    if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                        yel_parse_statement(yel_tokens, opcodes);
                    } else break;
                }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                    yel_parse_expression(yel_tokens, opcodes);
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
                    } 
                    
                    if (fbrks == 0) break;
                    
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                    _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, opcodes);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, opcodes);
                    } else break;
                }
            }

            //printf("jump %d\nend_while:\npop\n", tmp_inst);
            opcodes->codes[opcodes->len] = OP_JUMP_TO;
            opcodes->codes[opcodes->len+1] = tmp_inst-1;
            opcodes->len += 2;
            opcodes->codes[_NextAddress] = opcodes->len;
            opcodes->codes[opcodes->len] = POP_VALUE;
            
            while (_NextAddress < opcodes->len) {
                if (opcodes->codes[_NextAddress] == OP_BRK) {
                    opcodes->codes[_NextAddress] = opcodes->len;
                }
                else if (opcodes->codes[_NextAddress] == OP_CNT) {
                    opcodes->codes[_NextAddress] = while_start;
                }

                ++_NextAddress;
            }

            ++opcodes->len;
            return;
        }
        else if (_CurType == tok_word_func) {
            size_t tmp_i = ++yel_tokens->pointer;

            if (_NextType != tok_op_lpar) {
                yel_print_error("SyntaxError", "invalid syntax", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                return;
            }

            yel_tokens->pointer += 2;

            printf("%s:\n", yel_tokens->value[tmp_i]);
            int largCounter = 0;
            while (_CurType != tok_op_rpar) {
                if (_CurType == tok_name && (_NextType == tok_comma || _NextType == tok_op_rpar)) {
                    printf("%s      ; %d\n", _CurVal, ++largCounter);
                }
                else if (_CurType == tok_comma && _NextType == tok_name) {
                    ++yel_tokens->pointer;
                    continue;
                }
                else {
                    yel_print_error("SyntaxError", "invalid syntax", yel_tokens->src_ptr, 
                        yel_tokens->line[yel_tokens->pointer], 
                        yel_tokens->start_symbol[yel_tokens->pointer]);
                }

                ++yel_tokens->pointer;
            }
            ++yel_tokens->pointer;

            if (_CurType == tok_op_flbrk) {
                int fbrks = 0;

                while (yel_tokens->pointer < yel_tokens->length) {
                    if (_CurType == tok_op_flbrk) {
                        ++yel_tokens->pointer;
                        ++fbrks;
                    }  else if (_CurType == tok_op_frbrk) {
                        ++yel_tokens->pointer;
                        --fbrks;
                    } 
                    
                    if (fbrks == 0) break;
                    
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                    _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, opcodes);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, opcodes);
                    } else break;
                }

                printf("ret\n");
            }
            else {
                yel_print_error("SyntaxError", "invalid syntax. function must be declared and implemented", 
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);
            }

            printf("\nload_const [func address]\n");
            printf("make_func_and_store %s\n", yel_tokens->value[tmp_i]);

            opcodes->len += 4;
            return;
        }

        ++yel_tokens->pointer;
    }

    opcodes->codes = realloc(opcodes->codes, (opcodes->len+1)*sizeof(OPCODEWORD));
    opcodes->codes[opcodes->len] = OP_HALT;
    ++opcodes->len;
}
