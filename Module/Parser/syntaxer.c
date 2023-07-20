#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"
#include "../YVM/yvm.h"

#define print_ystack() for(int i=0;i<_ParserStackCounter;i++)printf("%d\n",_ParserStack[i]);

#define isStmt(t)       ((t) >= stmt && (t) <= stmt_while)
#define isExpr(t)       ((t) >= expr && (t) <= expr_func_call)
#define isUndefined(t)  (isStmt(t) || isExpr(t))
#define isTypeWord(t)   ((t) >= tok_word_Int && (t) <= tok_word_Str)
#define isType(t)       ((t) == expr_name || isTypeWord(t))

YelTokenType _ParserStack[1024];
int _ParserStackCounter = 0;
static int _Parentheses = 0;
static _Bool _Unary = 1;

// from syntaxer.c
// fix
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
    _Unary = 1;

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
        ((_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_log_or || _NextType == tok_comma || 
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

            if (_Parentheses == 0) {
                break;
            }
        }
        else if (_CurType == tok_op_lpar && (_NextType == tok_number_int || _NextType == tok_number_flt || 
        _NextType == tok_bool || _NextType == tok_name || _NextType == tok_string ||  
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_not || _NextType == tok_unary_op_log_not || 
        _NextType == tok_op_lpar || _NextType == tok_op_rpar)) {
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
        else if ((_CurType >= tok_number_int && _CurType <= tok_bool) && 
        (_NextType >= tok_binary_op_pow && _NextType <= tok_binary_op_assign || 
        _NextType == tok_comma || _NextType == tok_semicolon || _NextType == tok_op_rpar)) {
            _ParserStack[_ParserStackCounter] = tok_en_expr;
            ++_ParserStackCounter;
            _Unary = 0;
        }
        else if ((_CurType == tok_unary_op_not || _CurType == tok_unary_op_log_not || _CurType == tok_unary_op_pos || _CurType == tok_unary_op_neg) && _Unary && 
        (_NextType >= tok_name && _NextType <= tok_bool ||
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_not || _NextType == tok_op_lpar || _NextType == tok_unary_op_log_not)) {
            _ParserStack[_ParserStackCounter] = tok_unary_op;
            ++_ParserStackCounter;
        }
        else if (_CurType >= tok_binary_op_pow && _CurType <= tok_binary_op_assign && 
        (_NextType >= tok_name && _NextType <= tok_bool ||
        _NextType == tok_binary_op_plus || _NextType == tok_binary_op_minus ||
        _NextType == tok_unary_op_log_not || _NextType == tok_op_lpar) ) {
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

    // print_ystack();
    // getchar();
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
void yel_gen_opcode(YelTokens* yel_tokens, YelByteCode* bytecode) {
    bytecode->opcode = (OPCODEWORD*)__builtin_malloc(64*sizeof(OPCODEWORD));
    bytecode->len = 0;

    while (yel_tokens->pointer < yel_tokens->length) {
        YelParsingEntities tmp;
        switch (tmp = yel_define_next_entity(yel_tokens)) {
        case expr_name:
            puts("<expr_name>");
            break;
        case expr_num:
            puts("<expr_num>");
            break;
        case expr_bool:
            puts("<expr_bool>");
            break;
        case expr_str:
            puts("<expr_str>");
            break;
        case expr_bin_op:
            puts("<expr_bin_op>");
            break;
        case expr_unary_op:
            puts("<expr_unary_op>");
            break;
        case expr_enumeration:
            puts("<expr_enumeration>");
            break;
        case expr_func_decl:
            puts("<expr_func_decl>");
            break;
        case expr_brackets:
            puts("<expr_brackets>");
            break;
        case expr_func_call:
            puts("<expr_func_call>");
            break;

        case expr:
            yel_parse_expression(yel_tokens, bytecode);
            break;

        case undefined:
            yel_tokens->error = 1;
            goto _end;

        default:
            printf("tmp = %d\n", tmp);
            break;
        }
    }

_end:
    bytecode->opcode[bytecode->len] = OP_HALT;
    ++bytecode->len;
}

YelTokenType def_stack[2048];
size_t dscounter = 0;
YelParsingEntities yel_define_next_entity(YelTokens* yel_tokens) {
    while (1) {
        switch (_CurType) {
        case tok_number_int:
        case tok_number_flt:
            def_stack[dscounter] = expr_num;
            break;

        case tok_bool_true:
        case tok_bool_false:
            def_stack[dscounter] = expr_bool;
            break;

        case tok_name:
            def_stack[dscounter] = expr_name;
            break;

        case tok_string:
            def_stack[dscounter] = expr_str;
            break;

        case tok_word_break:
            def_stack[dscounter] = stmt_break;
            break;

        case tok_word_continue:
            def_stack[dscounter] = stmt_continue;
            break;

        default:
            if (_CurType >= tok_binary_op_pow && _CurType <= tok_binary_op_log_or) {
                def_stack[dscounter] = bin_op;
            }
            else if (_CurType == tok_unary_op_pos || _CurType == tok_unary_op_neg || (_CurType >= tok_unary_op_log_not && _CurType <= tok_unary_op_not)) {
                def_stack[dscounter] = unary_op;
            }
            else if (_CurType >= tok_binary_op_div_assign && _CurType <= tok_binary_op_assign) {
                def_stack[dscounter] = assign_op;
            }
            else if (_CurType >= tok_word_Int && _CurType <= tok_word_Str) {
                def_stack[dscounter] = type_word;
            }
            else {
                def_stack[dscounter] = _CurType;
            }

            break;
        }

        if (_CurType == tok_semicolon) {
            ++yel_tokens->pointer;
            ++dscounter;
            break;
        }

        ++yel_tokens->pointer;
        ++dscounter;
    }

    size_t sp = 0;
    int tmp_sp;
    while (sp < dscounter) {
        // stmt_assign
        if (def_stack[sp] == stmt_name_decl && def_stack[sp+1] == assign_op && isExpr(def_stack[sp+2])) {
            def_stack[sp] = stmt_assign;
            goto _delete2;
        }

        // stmt_name_decl
        else if (isExpr(def_stack[sp]) && def_stack[sp+1] == type_annotation) {
            def_stack[sp] = stmt_name_decl;
            goto _delete1;
        }

        // type_annotation
        else if (def_stack[sp] == tok_colon && isType(def_stack[sp+1])) {
            def_stack[sp] = type_annotation;
            goto _delete1;
        }

        // stmt_name_decl_enum
        else if (def_stack[sp] == stmt_name_decl && def_stack[sp+1] == tok_comma && def_stack[sp+2] == stmt_name_decl) {
            def_stack[sp] = stmt_name_decl_enum;
            goto _delete2;
        }

        // stmt_return
        else if (def_stack[sp] == tok_word_return && isExpr(def_stack[sp+1])) {
            def_stack[sp] = stmt_return;
            goto _delete1;
        }

        // stmt_break
        else if (def_stack[sp] == tok_word_break) {
            def_stack[sp] = stmt_break;
            goto _delete0;
        }

        // stmt_continue
        else if (def_stack[sp] == tok_word_continue) {
            def_stack[sp] = stmt_continue;
            goto _delete0;
        }

        // stmt_if
        else if (def_stack[sp] == tok_word_if && def_stack[sp+1] == expr_brackets && isUndefined(def_stack[sp+2])) {
            def_stack[sp] = stmt_if;
            goto _delete2;
        }

        // stmt_if_else
        else if (def_stack[sp] == stmt_if && def_stack[sp+1] == tok_word_else && isUndefined(def_stack[sp+2])) {
            def_stack[sp] = stmt_if_else;
            goto _delete2;
        }

        // stmt_curly_bracket
        else if (def_stack[sp] == tok_op_flbrk && isUndefined(def_stack[sp+1]) && def_stack[sp+2] == tok_op_frbrk) {
            def_stack[sp] = stmt_curly_bracket;
            goto _delete2;
        }

        // stmt_while
        else if (def_stack[sp] == tok_word_while && def_stack[sp+1] == expr_brackets && isUndefined(def_stack[sp+2])) {
            def_stack[sp] = stmt_while;
            goto _delete2;
        }

        // expr_bin_op
        else if (isExpr(def_stack[sp]) && def_stack[sp+1] == bin_op && isExpr(def_stack[sp+2])) {
            def_stack[sp] = expr_bin_op;
            goto _delete2;
        }

        // expr_unary_op
        else if (def_stack[sp] == unary_op && isExpr(def_stack[sp+1])) {
            def_stack[sp] = expr_unary_op;
            goto _delete1;
        }

        // expr_enumeration
        else if (isExpr(def_stack[sp]) && def_stack[sp+1] == tok_comma && isExpr(def_stack[sp+2])) {
            def_stack[sp] = expr_enumeration;
            goto _delete2;
        }

        // expr_brackets
        else if (def_stack[sp] == tok_op_lpar && isExpr(def_stack[sp+1]) && def_stack[sp+2] == tok_op_rpar) {
            def_stack[sp] = expr_brackets;
            goto _delete2;
        }
        else if (def_stack[sp] == tok_op_lpar && def_stack[sp+1] == tok_op_rpar) {
            def_stack[sp] = expr_brackets;
            goto _delete1;
        }

        // expr_func_call
        else if (isExpr(def_stack[sp]) && def_stack[sp+1] == expr_brackets) {
            def_stack[sp] = expr_func_call;
            goto _delete1;
        }

_delete0:
        goto _continue;

_delete1:
        tmp_sp = sp+1;
        dscounter -= 1;
        while (tmp_sp < dscounter) {
            def_stack[tmp_sp] = def_stack[tmp_sp+1];
            ++tmp_sp;
        }
        goto _continue;

_delete2:
        tmp_sp = sp+1;
        dscounter -= 2;
        while (tmp_sp < dscounter) {
            def_stack[tmp_sp] = def_stack[tmp_sp+2];
            ++tmp_sp;
        }

_continue:
        if (sp <= 1) {
            break;
        }

        sp = 0;
        continue;
    }

    return def_stack[0];
}