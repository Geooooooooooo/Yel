#include "../Parser/parser.h"
#include "../Parser/syntaxer.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"
#include "../Dependencies/dependencies.h"
#include "../VM/yvm.h"

#define WRITE_OPCODE() {\
    if (_CurType == tok_name) {bytecode->opcode[bytecode->len] = LOAD_VALUE;\
        bytecode->opcode[bytecode->len+1] = (OPCODEWORD)_CurVal;}\
    else if (_CurType == tok_number_int) {bytecode->opcode[bytecode->len] = LOAD_CONST;\
        bytecode->opcode[bytecode->len+1] = yel_alloc_Int_data(atoll(_CurVal));}\
    else if(_CurType == tok_number_flt) {bytecode->opcode[bytecode->len] = LOAD_CONST;\
        bytecode->opcode[bytecode->len+1] = yel_alloc_Flt_data(atof(_CurVal));}\
    else if(_CurType == tok_bool) {bytecode->opcode[bytecode->len] = LOAD_CONST;\
        bytecode->opcode[bytecode->len+1] = yel_alloc_Bool_data((_Bool)atoi(_CurVal));}\
    else if(_CurType == tok_string) {bytecode->opcode[bytecode->len] = LOAD_CONST;\
        bytecode->opcode[bytecode->len+1] = yel_alloc_Str_data(_CurVal);}bytecode->len+=2;++argCounter;}

int          _SimpleExpr = 0;
static int   _Parentheses = 0;      // ()
static int  _Unary = 1;
static size_t argCounter = 0;
static size_t curInstLen = 0;

// from parser.c
void yel_parse_expression(YelTokens* yel_tokens, YelByteCode* bytecode) {
    if (curInstLen < bytecode->len+10) {
        curInstLen = bytecode->len + 20;
        bytecode->opcode = __builtin_realloc(bytecode->opcode, (curInstLen)*sizeof(OPCODEWORD));
    }

    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_semicolon) {            
            ++yel_tokens->pointer;
            _SimpleExpr = 0;
            return;
        }

        // ()
        else if (_CurType == tok_op_lpar) {
            ++_Parentheses;
            ++yel_tokens->pointer;
            _Unary = 1;

            register int tmp_SimpleExpr = _SimpleExpr;
            _SimpleExpr = 0;
            yel_parse_expression(yel_tokens, bytecode);
            _SimpleExpr = tmp_SimpleExpr;

            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if (_CurType == tok_op_rpar) {
            --_Parentheses;
            return;
        }

        // **
        else if (_NextType == tok_binary_op_pow) {
            WRITE_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            _Unary = 1;
            
            yel_parse_expression(yel_tokens, bytecode);
            
            --_SimpleExpr;

            if (_NextType == tok_binary_op_pow) {
                ++yel_tokens->pointer;
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_POW;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType == tok_binary_op_pow) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType == tok_binary_op_pow) {
                ++yel_tokens->pointer;
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_POW;
            bytecode->len += 2;

            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        
        else if (_Unary && (_CurType == tok_binary_op_plus || _CurType == tok_binary_op_minus || _CurType == tok_unary_op_not ||
        _CurType == tok_unary_op_inc || _CurType == tok_unary_op_dec)) {
            size_t uo_ptr = yel_tokens->pointer;
            ++_SimpleExpr;
            ++yel_tokens->pointer;
            
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType == tok_binary_op_pow) {
                ++yel_tokens->pointer;
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            if (yel_tokens->type[uo_ptr] == tok_unary_op_pos)
                bytecode->opcode[bytecode->len] = UNARY_POS;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_neg)
                bytecode->opcode[bytecode->len] = UNARY_NEG;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_inc)
                bytecode->opcode[bytecode->len] = UNARY_INC;
            else if(yel_tokens->type[uo_ptr] == tok_unary_op_dec)
                bytecode->opcode[bytecode->len] = UNARY_DEC;
            else if (yel_tokens->type[uo_ptr] == tok_unary_op_not) 
                bytecode->opcode[bytecode->len] = UNARY_NOT;

            ++bytecode->len;
            _Unary = 0;
            if (_SimpleExpr) return;
        }

        // *, /, %
        else if(_NextType == tok_binary_op_div || _NextType == tok_binary_op_mul || _NextType == tok_binary_op_percent) {
            _Unary = 1;
            
            WRITE_OPCODE();
            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            
            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType == tok_binary_op_pow) {
                _Unary = 0;

                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                bytecode->opcode[bytecode->len+1] = BYNARY_DIV;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                bytecode->opcode[bytecode->len+1] = BYNARY_MUL;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                bytecode->opcode[bytecode->len+1] = BYNARY_MOD;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if(_CurType == tok_binary_op_div || _CurType == tok_binary_op_mul || _CurType == tok_binary_op_percent) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;
            
            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType == tok_binary_op_pow) {
                _Unary = 0;

                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_div)
                bytecode->opcode[bytecode->len+1] = BYNARY_DIV;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_mul)
                bytecode->opcode[bytecode->len+1] = BYNARY_MUL;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_percent)
                bytecode->opcode[bytecode->len+1] = BYNARY_MOD;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        
        // +, -
        else if ((_NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus) 
        //&& (yel_tokens->pointer == 0 || 
        //(_CurType != tok_op_lpar &&  _CurType < tok_binary_op_pow && _CurType > tok_binary_op_log_or))
        ) {
            _Unary = 1;
            WRITE_OPCODE();

            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;
            
            yel_parse_expression(yel_tokens, bytecode);
            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_percent) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                bytecode->opcode[bytecode->len+1] = BYNARY_ADD;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                bytecode->opcode[bytecode->len+1] = BYNARY_SUB;
                
            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if ((_CurType == tok_binary_op_plus || _CurType == tok_binary_op_minus)) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_percent) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_plus)
                bytecode->opcode[bytecode->len+1] = BYNARY_ADD;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_minus)
                bytecode->opcode[bytecode->len+1] = BYNARY_SUB;
                
            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
            if (_SimpleExpr) return;
        }
        
        // <<, >>
        else if (_NextType == tok_binary_op_rsh || _NextType == tok_binary_op_lsh) {
            _Unary = 1;

            WRITE_OPCODE();
            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_minus) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                bytecode->opcode[bytecode->len+1] = BYNARY_RSH;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                bytecode->opcode[bytecode->len+1] = BYNARY_LSH;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType == tok_binary_op_rsh || _CurType == tok_binary_op_lsh) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_minus) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_rsh)
                bytecode->opcode[bytecode->len+1] = BYNARY_RSH;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_lsh)
                bytecode->opcode[bytecode->len+1] = BYNARY_LSH;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }

        // &
        else if (_NextType == tok_binary_op_and) {
            _Unary = 1;

            WRITE_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);
            --_SimpleExpr;

            if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_lsh) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_AND;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType == tok_binary_op_and) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_lsh) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }
            
            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_AND;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        
        // |
        else if (_NextType == tok_binary_op_or) {
            _Unary = 1;

            WRITE_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_OR;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType == tok_binary_op_or) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                _Unary = 0;

                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }
            
            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_OR;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        // <, <=, >, >=, ==, !=
        else if (_NextType >= tok_binary_op_more && _NextType <= tok_binary_op_not_eq) {
            _Unary = 1;

            WRITE_OPCODE();
            if (_SimpleExpr) return;

            size_t tmp_i = yel_tokens->pointer + 1;
            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_more) 
                bytecode->opcode[bytecode->len+1] = BYNARY_MORE;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less) 
                bytecode->opcode[bytecode->len+1] = BYNARY_LESS;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_more_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_MORE_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_LESS_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_not_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_NOT_EQ;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType >= tok_binary_op_more && _CurType <= tok_binary_op_not_eq) {
            size_t tmp_i = yel_tokens->pointer;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;

            if (yel_tokens->type[tmp_i] == tok_binary_op_more) 
                bytecode->opcode[bytecode->len+1] = BYNARY_MORE;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less) 
                bytecode->opcode[bytecode->len+1] = BYNARY_LESS;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_more_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_MORE_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_less_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_LESS_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_EQ;
            else if (yel_tokens->type[tmp_i] == tok_binary_op_not_eq)
                bytecode->opcode[bytecode->len+1] = BYNARY_NOT_EQ;
                
            bytecode->len += 2;
            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        // not
        else if (_Unary && _CurType == tok_unary_op_log_not) {
            size_t uo_ptr = yel_tokens->pointer;
            ++_SimpleExpr;
            ++yel_tokens->pointer;
            
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_not_eq) {
                ++yel_tokens->pointer;
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = UNARY_LOGICAL_NOT;

            ++bytecode->len;
            _Unary = 0;
            if (_SimpleExpr) return;
        }

        // and
        else if (_NextType == tok_binary_op_log_and) {
            _Unary = 1;

            WRITE_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_unary_op_log_not) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_LOGICAL_AND;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType == tok_binary_op_log_and) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_unary_op_log_not) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }
            
            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_LOGICAL_AND;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        
        // or
        else if (_NextType == tok_binary_op_log_or) {
            _Unary = 1;

            WRITE_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_LOGICAL_OR;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;
        }
        else if (_CurType == tok_binary_op_log_or) {
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_and) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }
            
            
            bytecode->opcode[bytecode->len] = BYNARY_OP;
            bytecode->opcode[bytecode->len+1] = BYNARY_LOGICAL_OR;

            bytecode->len += 2;
            --argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        // ,
        else if (_NextType == tok_comma) {
            _Unary = 1;
            
            WRITE_OPCODE();
            if (_SimpleExpr) return;

            yel_tokens->pointer += 2;
            ++_SimpleExpr;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            _Unary = 0;
            // if
            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }
        }
        else if (_CurType == tok_comma) {
            if (_SimpleExpr) return;
            ++yel_tokens->pointer;
            ++_SimpleExpr;
            _Unary = 1;

            yel_parse_expression(yel_tokens, bytecode);

            --_SimpleExpr;

            _Unary = 0;
            // if
            while (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_or) {
                ++_SimpleExpr;
                ++yel_tokens->pointer;
                yel_parse_expression(yel_tokens, bytecode);
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

                yel_parse_expression(yel_tokens, bytecode);

                --_SimpleExpr;

                bytecode->opcode[bytecode->len] = LOAD_VALUE;
                bytecode->opcode[bytecode->len+1] = yel_tokens->value[tmp_i];
                bytecode->opcode[bytecode->len+2] = OP_CALL;
                bytecode->opcode[bytecode->len+3] = argCounter;

                bytecode->len += 4;
                argCounter = tmp_argCounter + 1;
                _Unary = 0;
            }
            else if (_NextType == tok_binary_op_assign) {
                size_t tmp_i = yel_tokens->pointer;
                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens, bytecode);

                bytecode->opcode[bytecode->len] = DUP_VALUE;
                bytecode->opcode[bytecode->len+1] = OP_STORE;
                bytecode->opcode[bytecode->len+2] = yel_tokens->value[tmp_i];

                bytecode->len += 3;

                continue;
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_pow_assign) {
                if (_Unary == 2) {
                    yel_print_error("SyntaxError", "invalid syntax", yel_tokens->src_ptr, 
                        yel_tokens->line[yel_tokens->pointer+1], 
                        yel_tokens->start_symbol[yel_tokens->pointer+1]);

                    yel_tokens->error = 1;
                    return;
                }

                WRITE_OPCODE();

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = _NextType;

                yel_tokens->pointer += 2;

                yel_parse_expression(yel_tokens, bytecode);

                bytecode->opcode[bytecode->len] = BYNARY_OP;
                switch (tmp_type) {
                case tok_binary_op_div_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_DIV; break;
                case tok_binary_op_mul_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_MUL; break;
                case tok_binary_op_percent_assign:  bytecode->opcode[bytecode->len+1] = BYNARY_MOD; break;
                case tok_binary_op_plus_assign:     bytecode->opcode[bytecode->len+1] = BYNARY_ADD; break;
                case tok_binary_op_minus_assign:    bytecode->opcode[bytecode->len+1] = BYNARY_SUB; break;
                case tok_binary_op_rsh_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_RSH; break;
                case tok_binary_op_lsh_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_LSH; break;
                case tok_binary_op_and_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_AND; break;
                case tok_binary_op_or_assign:       bytecode->opcode[bytecode->len+1] = BYNARY_OR;  break;
                case tok_binary_op_pow_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_POW; break;
                }
                
                bytecode->opcode[bytecode->len+2] = DUP_VALUE;
                bytecode->opcode[bytecode->len+3] = OP_STORE;
                bytecode->opcode[bytecode->len+4] = yel_tokens->value[tmp_i];

                bytecode->len += 5;
                continue;
            }
            else {
                bytecode->opcode[bytecode->len] = LOAD_VALUE;
                bytecode->opcode[bytecode->len+1] = _CurVal;
                bytecode->len += 2;

                ++argCounter;
                _Unary = 0;

                // ???
                if (_SimpleExpr) return;
            }
        }
        else if(_CurType == tok_number_int) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                yel_print_error("SyntaxError", "expression on the left should not be a constant", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                yel_tokens->error = 1;
            }
            
            bytecode->opcode[bytecode->len] = LOAD_CONST;
            bytecode->opcode[bytecode->len+1] = yel_alloc_Int_data(atoll(_CurVal));            // add to const
            bytecode->len += 2;

            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_number_flt) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                yel_print_error("SyntaxError", "expression on the left should not be a constant", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                yel_tokens->error = 1;
            }
            
            bytecode->opcode[bytecode->len] = LOAD_CONST;
            bytecode->opcode[bytecode->len+1] = yel_alloc_Flt_data(atof(_CurVal));            // add to const
            bytecode->len += 2;

            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_string) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                yel_print_error("SyntaxError", "expression on the left should not be a constant", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                yel_tokens->error = 1;
            }
            
            bytecode->opcode[bytecode->len] = LOAD_CONST;
            bytecode->opcode[bytecode->len+1] = yel_alloc_Str_data(_CurVal);            // add to const
            bytecode->len += 2;

            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }
        else if(_CurType == tok_bool) {
            if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) {
                yel_print_error("SyntaxError", "expression on the left should not be a constant", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);

                yel_tokens->error = 1;
            }
            
            bytecode->opcode[bytecode->len] = LOAD_CONST;

            if (strcmp(_CurVal, "True") == 0)
                bytecode->opcode[bytecode->len+1] = yel_alloc_Bool_data((_Bool)1);
            else
                bytecode->opcode[bytecode->len+1] = yel_alloc_Bool_data((_Bool)0);

            bytecode->len += 2;

            ++argCounter;
            _Unary = 0;

            if (_SimpleExpr) return;
        }

        //if (_CurType == tok_op_rpar) continue;
        ++yel_tokens->pointer;
        //if (_NextType == tok_semicolon) return;
    }
}

// from parser.c
void yel_parse_statement(YelTokens* yel_tokens, YelByteCode* bytecode) {
    if (curInstLen < bytecode->len+10) {
        curInstLen = bytecode->len + 40;
        bytecode->opcode = __builtin_realloc(bytecode->opcode, (curInstLen)*sizeof(OPCODEWORD));
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

                yel_parse_expression(yel_tokens, bytecode);
                
                bytecode->opcode[bytecode->len] = OP_STORE;
                bytecode->opcode[bytecode->len+1] = (OPCODEWORD)yel_tokens->value[tmp_i];
                bytecode->len += 2;
            }
            else if (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_pow_assign) {
                
                bytecode->opcode[bytecode->len] = LOAD_VALUE;
                bytecode->opcode[bytecode->len+1] = (OPCODEWORD)_CurVal;
                bytecode->len += 2;

                size_t tmp_i = yel_tokens->pointer;
                YelTokenType tmp_type = _NextType;

                yel_tokens->pointer += 2;

                _Unary = 1;
                yel_parse_expression(yel_tokens, bytecode);
                _Unary = 0;

                bytecode->opcode[bytecode->len] = BYNARY_OP;
                switch (tmp_type) {
                case tok_binary_op_div_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_DIV; break;
                case tok_binary_op_mul_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_MUL; break;
                case tok_binary_op_percent_assign:  bytecode->opcode[bytecode->len+1] = BYNARY_MOD; break;
                case tok_binary_op_plus_assign:     bytecode->opcode[bytecode->len+1] = BYNARY_ADD; break;
                case tok_binary_op_minus_assign:    bytecode->opcode[bytecode->len+1] = BYNARY_SUB; break;
                case tok_binary_op_rsh_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_RSH; break;
                case tok_binary_op_lsh_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_LSH; break;
                case tok_binary_op_and_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_AND; break;
                case tok_binary_op_or_assign:       bytecode->opcode[bytecode->len+1] = BYNARY_OR;  break;
                case tok_binary_op_pow_assign:      bytecode->opcode[bytecode->len+1] = BYNARY_POW; break;
                }

                bytecode->opcode[bytecode->len+2] = OP_STORE;
                bytecode->opcode[bytecode->len+3] = (OPCODEWORD)yel_tokens->value[tmp_i];

                bytecode->len += 4;
            }

            _Unary = 1;
            return;
        }
        else if (_CurType == tok_word_return) {
            ++yel_tokens->pointer;
            _Unary = 1;
            yel_parse_expression(yel_tokens, bytecode);

            bytecode->opcode[bytecode->len] = OP_RET;
            ++bytecode->len;
            return;
        }
        else if (_CurType == tok_word_defer) {       // NO REALIZED
            puts("defer:");
            ++yel_tokens->pointer;
            yel_parse_expression(yel_tokens, bytecode);
            printf("ret\n");
            return;
        }
        else if (_CurType == tok_word_break) {
            bytecode->opcode[bytecode->len] = OP_JUMP_TO;
            bytecode->opcode[bytecode->len+1] = OP_BRK;

            bytecode->len += 2;

            yel_tokens->pointer += 2;
            return;
        }
        else if (_CurType == tok_word_continue) {
            bytecode->opcode[bytecode->len] = OP_JUMP_TO;
            bytecode->opcode[bytecode->len+1] = OP_CNT;

            bytecode->len += 2;

            yel_tokens->pointer += 2;
            return;
        }
        else if (_CurType == tok_word_if) {
            ++yel_tokens->pointer;
            if (yel_check_expr(yel_tokens, 1) == RET_CODE_OK) {
                _Unary = 1;
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }
            
            bytecode->opcode[bytecode->len] = OP_POP_JUMP_ZERO;
            size_t _NextAddress = bytecode->len+1;

            bytecode->len += 2;

            ++yel_tokens->pointer;

            if (_CurType != tok_op_flbrk) {
                if (_CurType == tok_name &&(_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                _CurType == tok_word_break || 
                _CurType == tok_word_return || _CurType == tok_op_flbrk || _CurType == tok_op_frbrk || 
                _CurType == tok_word_if || _CurType == tok_word_while || _CurType == tok_word_continue) {
                    if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                        yel_parse_statement(yel_tokens, bytecode);
                    } else break;
                }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                    yel_parse_expression(yel_tokens, bytecode);
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
                    _CurType == tok_word_if || _CurType == tok_word_while|| _CurType == tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, bytecode);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, bytecode);
                    } else break;
                }
            }

            if (_CurType == tok_word_else) {
                bytecode->opcode[_NextAddress] = bytecode->len+2;
                
                bytecode->opcode[bytecode->len] = OP_JUMP_TO;
                _NextAddress = bytecode->len+1;

                bytecode->len += 2;

                ++yel_tokens->pointer;

                if (_CurType != tok_op_flbrk) {
                    if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                    _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                    _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| _CurType == tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, bytecode);
                        } else break;
                    }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, bytecode);
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
                        _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| _CurType == tok_word_continue) {
                            if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                                yel_parse_statement(yel_tokens, bytecode);
                            } else break;
                        } 
                        else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                            yel_parse_expression(yel_tokens, bytecode);
                        } else break;
                    }
                }

                bytecode->opcode[_NextAddress] = bytecode->len;            
            }
            else {
                bytecode->opcode[_NextAddress] = bytecode->len;
            }

            return;
        }
        else if (_CurType == tok_word_while) {
            ++yel_tokens->pointer;

            size_t while_start = bytecode->len;

            if (yel_check_expr(yel_tokens, 1) == RET_CODE_OK) {
                ++_SimpleExpr;
                yel_parse_expression(yel_tokens, bytecode);
                --_SimpleExpr;
            }

            bytecode->opcode[bytecode->len] = OP_POP_JUMP_ZERO;
            size_t _NextAddress = bytecode->len+1;

            bytecode->len += 2;

            ++yel_tokens->pointer;

            if (_CurType != tok_op_flbrk) {
                if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
                _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || 
                _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while || _CurType == tok_word_continue) {
                    if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                        yel_parse_statement(yel_tokens, bytecode);
                    } else break;
                }  else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                    yel_parse_expression(yel_tokens, bytecode);
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
                    _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while|| _CurType == tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, bytecode);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, bytecode);
                    } else break;
                }
            }

            bytecode->opcode[bytecode->len] = OP_JUMP_TO;
            bytecode->opcode[bytecode->len+1] = (OPCODEWORD)while_start;

            bytecode->len += 2;
            bytecode->opcode[_NextAddress] = (OPCODEWORD)bytecode->len;
            
            while (_NextAddress < bytecode->len) {
                if (bytecode->opcode[_NextAddress] == OP_BRK)
                    bytecode->opcode[_NextAddress] = bytecode->len;
                else if (bytecode->opcode[_NextAddress] == OP_CNT)
                    bytecode->opcode[_NextAddress] = while_start;

                ++_NextAddress;
            }
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
                    _CurType == tok_op_frbrk || _CurType == tok_word_if || _CurType == tok_word_while||  _CurType == tok_word_continue) {
                        if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                            yel_parse_statement(yel_tokens, bytecode);
                        } else break;
                    } 
                    else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
                        yel_parse_expression(yel_tokens, bytecode);
                    } else break;
                }

                printf("ret\n");
            }
            else {
                yel_print_error("SyntaxError", "invalid syntax. function must be declared and implemented", 
                    yel_tokens->src_ptr, yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);
            }

            bytecode->opcode[bytecode->len] = LOAD_CONST;
            bytecode->opcode[bytecode->len+1] = yel_tokens->value[tmp_i];

            bytecode->opcode[bytecode->len+2] = MAKE_FUNC;
            bytecode->opcode[bytecode->len+3] = OP_STORE;
            bytecode->opcode[bytecode->len+4] = yel_tokens->value[tmp_i];

            bytecode->len += 5;
            return;
        }

        ++yel_tokens->pointer;
    }

    // bytecode->opcode = __builtin_realloc(bytecode->opcode, (bytecode->len+1)*sizeof(OPCODEWORD));
    //bytecode->opcode[bytecode->len] = OP_HALT;
    //++bytecode->len;
}