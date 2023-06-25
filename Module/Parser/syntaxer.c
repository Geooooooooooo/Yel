#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"

#define print_ystack() for(int i=0;i<_ParserStackCounter;i++)printf("%d\n",_ParserStack[i]);

YelTokenType _ParserStack[1024];
int _ParserStackCounter = 0;
static int _Parentheses = 0;
static _Bool _Unary = 1;

// from syntaxer.c
_Bool yel_check_expr_grammar(YelTokens* yel_tokens) {
    register int sp = 0;

    while (sp < _ParserStackCounter) {
        if (_ParserStack[sp] == tok_en_expr && _ParserStack[sp+1] == tok_binary_op && _ParserStack[sp+2] == tok_en_expr) {
            register int tmp_sp = sp+1;
            _ParserStackCounter -= 2;

            while (tmp_sp < _ParserStackCounter) {
                _ParserStack[tmp_sp] = _ParserStack[tmp_sp+2];
                ++tmp_sp;
            }

            sp = 0;
            continue;
        }
        else if (_ParserStack[sp] == tok_en_expr && _ParserStack[sp+1] == tok_comma && _ParserStack[sp+2] == tok_en_expr) {
            register int tmp_sp = sp+1;
            _ParserStackCounter -= 2;

            while (tmp_sp < _ParserStackCounter) {
                _ParserStack[tmp_sp] = _ParserStack[tmp_sp+2];
                ++tmp_sp;
            }

            sp = 0;
            continue;
        }
        else if (_ParserStack[sp] == tok_op_lpar) {
            if (_ParserStack[sp+1] == tok_en_expr && _ParserStack[sp+2] == tok_op_rpar ) {
                _ParserStack[sp] = tok_en_expr;
                register int tmp_sp = sp+1;
                _ParserStackCounter -= 2;

                while (tmp_sp < _ParserStackCounter) {
                    _ParserStack[tmp_sp] = _ParserStack[tmp_sp+2];
                    ++tmp_sp;
                }

                sp = 0;
                continue;
            }
            else if (_ParserStack[sp+1] == tok_op_rpar) {
                _ParserStack[sp] = tok_en_expr;
                register int tmp_sp = sp+1;
                --_ParserStackCounter;

                while (tmp_sp < _ParserStackCounter) {
                    _ParserStack[tmp_sp] = _ParserStack[tmp_sp+1];
                    ++tmp_sp;
                }

                sp = 0;
                continue;
            }
        }
        else if (_ParserStack[sp] == tok_unary_op) {
            if (_ParserStack[sp+1] == tok_en_expr) {
                _ParserStack[sp] = tok_en_expr;
                register int tmp_sp = sp+1;
                --_ParserStackCounter;

                while (tmp_sp < _ParserStackCounter) {
                    _ParserStack[tmp_sp] = _ParserStack[tmp_sp+1];
                    ++tmp_sp;
                }

                sp = 0;
                continue;
            }
        }
        else if (_ParserStack[sp] == tok_name && _ParserStack[sp+1] == tok_op_lpar) {
            if (_ParserStack[sp+2] == tok_en_expr && _ParserStack[sp+3] == tok_op_rpar) {
                _ParserStack[sp] = tok_en_expr;

                register int tmp_sp = sp+1;
                _ParserStackCounter -= 3;

                while (tmp_sp < _ParserStackCounter) {
                    _ParserStack[tmp_sp] = _ParserStack[tmp_sp+3];
                    ++tmp_sp;
                } 
                sp = 0;
            }
            else if (_ParserStack[sp+2] == tok_op_rpar) {
                _ParserStack[sp] = tok_en_expr;

                register int tmp_sp = sp+1;
                _ParserStackCounter -= 2;

                while (tmp_sp < _ParserStackCounter) {
                    _ParserStack[tmp_sp] = _ParserStack[tmp_sp+2];
                    ++tmp_sp;
                } 
                sp = 0;
            }
            else {
                sp += 2;
            }

            continue;
        }
        else if (_ParserStack[sp] == tok_op_rpar) {
            return RET_CODE_OK;
        }
            
        ++sp;
    }

    _ParserStackCounter = 0;
    return RET_CODE_OK;
}

// from syntaxer.c
_Bool yel_check_expr(YelTokens* yel_tokens, int stmt) {
    size_t start_pointer = yel_tokens->pointer;

    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_semicolon) {
            if (_Parentheses != 0) {
                yel_print_error("SyntaxError", "expected ')'", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer-1], 
                    yel_tokens->start_symbol[yel_tokens->pointer-1]);

                return RET_CODE_ERROR;
            }

            break;
        }
        else if (_CurType == tok_op_rpar && 
        ((_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_or ||  _NextType == tok_comma || 
        _NextType == tok_semicolon ||  _NextType == tok_op_rpar) || stmt)) {
            if (_Parentheses == 0) {
                yel_print_error("SyntaxError", "expected '('", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer], 
                    yel_tokens->start_symbol[yel_tokens->pointer]);
                return RET_CODE_ERROR;
            }

            --_Parentheses;
            _ParserStack[_ParserStackCounter] = tok_op_rpar;
            ++_ParserStackCounter;
            _Unary = 0;

            if (_Parentheses == 0) break;
        }
        else if (_CurType == tok_op_lpar && (_NextType == tok_number_int || _NextType == tok_number_flt || 
        _NextType == tok_bool || _NextType == tok_name || _NextType == tok_string ||  
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_not || _NextType == tok_op_lpar || _NextType == tok_op_rpar)) {
            _ParserStack[_ParserStackCounter] = tok_op_lpar;
            ++_ParserStackCounter;
            ++_Parentheses;
            _Unary = 1;
        }
        else if (_CurType == tok_name) {    
            if (_NextType == tok_op_lpar) {
                _ParserStack[_ParserStackCounter] = tok_name;
                ++_ParserStackCounter;
            }
            else if (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_assign || 
            _NextType == tok_comma || _NextType == tok_semicolon || _NextType == tok_op_rpar) {
                _ParserStack[_ParserStackCounter] = tok_en_expr;
                ++_ParserStackCounter;
                _Unary = 0;
            }
            else {
                yel_print_error("SyntaxError", "operator is expected", yel_tokens->src_ptr, 
                    yel_tokens->line[yel_tokens->pointer+1], 
                    yel_tokens->start_symbol[yel_tokens->pointer+1]);

                return RET_CODE_ERROR;
            }
        }
        else if ((_CurType == tok_number_int || _CurType == tok_number_flt || _CurType == tok_bool || _CurType == tok_string) && 
        (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_assign || 
        _NextType == tok_comma || _NextType == tok_semicolon || _NextType == tok_op_rpar)) {
            _ParserStack[_ParserStackCounter] = tok_en_expr;
            ++_ParserStackCounter;
            _Unary = 0;
        }
        else if ((_CurType == tok_unary_op_not || _CurType == tok_unary_op_pos || _CurType == tok_unary_op_neg) && _Unary && 
        (_NextType >= tok_name && _NextType <= tok_bool ||
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_not || _NextType == tok_op_lpar)) {
            _ParserStack[_ParserStackCounter] = tok_unary_op;
            ++_ParserStackCounter;
        }
        else if (_CurType >= tok_binary_op_pow && _CurType <= tok_binary_op_assign && 
        (_NextType >= tok_name && _NextType <= tok_bool ||
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_not || _NextType == tok_op_lpar) ) {
            _ParserStack[_ParserStackCounter] = tok_binary_op;
            ++_ParserStackCounter;
            _Unary = 1;
        }
        else if (_CurType == tok_comma && (_NextType >= tok_name && _NextType <= tok_bool ||
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_not || _NextType == tok_op_lpar)) {
            _ParserStack[_ParserStackCounter] = tok_comma;
            ++_ParserStackCounter;
            _Unary = 1;
        }
        else {
            yel_print_error("SyntaxError", "invalid syntax", yel_tokens->src_ptr, 
                yel_tokens->line[yel_tokens->pointer+1], 
                yel_tokens->start_symbol[yel_tokens->pointer+1]);

            return RET_CODE_ERROR;
        }
        
        ++yel_tokens->pointer;
    }

    yel_tokens->pointer = start_pointer;

    //print_ystack();
    //getchar();
    return yel_check_expr_grammar(yel_tokens);
}

// from syntaxer.c
_Bool yek_check_stmt(YelTokens* yel_tokens) {
    size_t start_pointer = yel_tokens->pointer;

    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign)) {
            yel_tokens->pointer += 2;

            if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) { break; }
            else { return RET_CODE_ERROR; }
        }
        else if (_CurType == tok_word_return) {
            ++yel_tokens->pointer;

            if (_NextType == tok_semicolon || yel_check_expr(yel_tokens, 0) == RET_CODE_OK) { break; }
            else { return RET_CODE_ERROR; }
        }
        else if (_CurType == tok_word_break && _NextType == tok_semicolon) {
            break;
        }
        else if (_CurType == tok_word_continue && _NextType == tok_semicolon) {
            break;
        }
        else if (_CurType == tok_op_flbrk || _CurType == tok_op_frbrk) {
            ++yel_tokens->pointer;
            break;
        }
        else if (_CurType == tok_word_if && _NextType == tok_op_lpar) {
            break;
        }
        else if (_CurType == tok_word_while && _NextType == tok_op_lpar) {
            break;
        }
        else if (_CurType == tok_word_func &&  _NextType == tok_name) {
            break;
        }
        else {
            yel_print_error("SyntaxError", "invalid syntax", yel_tokens->src_ptr, 
                yel_tokens->line[yel_tokens->pointer+1], 
                yel_tokens->start_symbol[yel_tokens->pointer+1]);

            return RET_CODE_ERROR;
        }

        ++yel_tokens->pointer;
    }

    yel_tokens->pointer = start_pointer;
    return RET_CODE_OK;
}

// from syntaxer.c
void yel_gen_opcode(YelTokens* yel_tokens, OPCODES* opcodes) {
    opcodes->codes = (OPCODEWORD*)malloc(100*sizeof(OPCODEWORD));
    opcodes->len = 0;

    while (yel_tokens->pointer < yel_tokens->length) {
        if (_CurType == tok_op_flbrk || _CurType == tok_op_frbrk) ++yel_tokens->pointer;
        
        if (_CurType == tok_name && (_NextType >= tok_binary_op_div_assign && _NextType <= tok_binary_op_assign) || 
        _CurType == tok_word_break ||  _CurType == tok_word_return || _CurType == tok_op_flbrk || _CurType == tok_op_frbrk || 
        _CurType == tok_word_if || _CurType == tok_word_while || _CurType == tok_word_func || _CurType == tok_word_continue) {
            if (yek_check_stmt(yel_tokens) == RET_CODE_OK) {
                yel_parse_statement(yel_tokens, opcodes);
            } else {
                yel_tokens->error = 1;
                break;
            }
        } 
        else if (yel_check_expr(yel_tokens, 0) == RET_CODE_OK) {
            yel_parse_expression(yel_tokens, opcodes);
        } else {
            yel_tokens->error = 1;
            break;
        }

    }

    opcodes->codes[opcodes->len] = OP_HALT;
    ++opcodes->len;
}
