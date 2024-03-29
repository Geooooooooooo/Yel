#include "../Lexer/lexer.h"
#include "../Errors/error.h"

#define cur_src     source->source_text
#define cur_ptr     source->pointer
#define cur_len     source->length
#define cur_char    cur_src[cur_ptr]

#define yel_is_op(c) (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ':')
#define yel_is_bin_op(c) (c == '*' || c == '+' || c == '-' || c == '/' || c == '>' || c == '<' || c == '=' || c == ',' || c == '~' || c == '^')
#define yel_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
#define yel_is_number(c) (c >= '0' && c <= '9')

// {}
size_t f_brk = 0;

size_t cur_line = 1;
size_t cur_line_symbol = 1;
size_t start_symbol = 1;

const int yel_keywords_length = 12;
const const char* yel_keywords[] = {
    "func", "return", "defer", "break",
    "if", "else", "while", "continue",
    "Int", "Flt", "Bool", "Str"
};

void yel_get_next_token(YelSource* source, YelTokenType* t_token_type, char* token_value) {
    register size_t token_value_counter = 0;

_start_:
    while ((cur_ptr < cur_len) && (cur_char == ' ' || cur_char == '\n' || cur_char == '\0')) {
        if (cur_char == '\n') {
            ++cur_line;
            cur_line_symbol = 0;
        }
        ++cur_ptr;
        ++cur_line_symbol;
    }

    if (cur_ptr == cur_len) {
        token_value[token_value_counter] = 0;
        *t_token_type = (YelTokenType)tok_semicolon;
        return;
    }

    // comments
    if (cur_char == '/' && cur_src[cur_ptr + 1] == '/') {
        while ((cur_ptr < cur_len) && cur_char != '\n') {
            ++cur_ptr;
        }

        cur_line_symbol = 0;
        goto _start_;
    }
    else if (cur_char == '/' && cur_src[cur_ptr + 1] == '*') {
        _Bool close_comment = 0;
        size_t com_start[2] = { cur_line, cur_line_symbol };

        while (cur_ptr < cur_len) {
            if (cur_char == '\n')  {
                ++cur_line;
                cur_line_symbol = 0;
            }
            if (cur_char == '*' && cur_src[cur_ptr + 1] == '/') {
                close_comment = 1;
                cur_ptr += 2;
                
                goto _start_;
            }

            ++cur_line_symbol;
            ++cur_ptr;
        }

        if (!close_comment) {
            printf("Lexical Error: module %s\n-->  %lu:%lu: unclosed comment \n|\n|", 
                source->file_name, com_start[0], com_start[1]);

            *t_token_type = tok_undefined;
            return;
        }
    }

    // num
    if (yel_is_number(cur_char) || cur_char == '.') {
        _Bool dot = 0;
        
        if (cur_char == '.') {
            1, token_value[token_value_counter] = cur_src[cur_ptr];
            ++cur_ptr;
            ++token_value_counter;
            start_symbol = cur_line_symbol;
            ++cur_line_symbol;
        } else start_symbol = cur_line_symbol;

        while (1) {
            if (cur_char == '.') {
                if (dot) {
                    yel_print_error("LexicalError", "invalid character in a numeric literal", source, cur_line, cur_line_symbol);
                    *t_token_type = tok_undefined;
                    return;
                }

                token_value[token_value_counter] = cur_char;
                ++token_value_counter;
                dot = 1;
            }
            else if(yel_is_number(cur_char)) {
                token_value[token_value_counter] = cur_char;
                ++token_value_counter;
            }
            else {
                if (cur_char == ' ' || cur_char == '\n' || yel_is_bin_op(cur_char) || yel_is_op(cur_char)) {
                    break;
                }
                else {
                    yel_print_error("LexicalError", "invalid character in a numeric literal", source, cur_line, cur_line_symbol);

                    *t_token_type = tok_undefined;
                    return;
                }
            }

            ++cur_ptr;
            ++cur_line_symbol;

        }
        if (dot) *t_token_type = (YelTokenType)tok_number_flt;
        else *t_token_type = (YelTokenType)tok_number_int;

        goto _end_;
    }

    // var
    if (yel_is_alpha(cur_char)) {
        start_symbol = cur_line_symbol;

        while (1) {
            if (yel_is_alpha(cur_char) || (cur_char >= '0' && cur_char <= '9')) {
                token_value[token_value_counter] = cur_char;
                ++token_value_counter;
            }
            else if (cur_char == ' ' || cur_char == '\n' || yel_is_bin_op(cur_char) || yel_is_op(cur_char)) {
                break;
            }
            else if (cur_char == '.') {
                yel_print_error("LexicalError", "invalid character", source, cur_line, cur_line_symbol);

                *t_token_type = tok_undefined;
                return;
            }

            ++cur_ptr;
            ++cur_line_symbol;
        }

        *t_token_type = (YelTokenType)tok_name;

        goto _end_;
    }

    // string
    if (cur_char == '\'' || cur_char == '\"') {
        start_symbol = cur_line_symbol;
        size_t com_start[2] = { cur_line, cur_line_symbol };

        char b = cur_char;
        ++cur_ptr;
        ++cur_line_symbol;

        while (cur_ptr <= cur_len) {
            if (cur_char == b) {
                ++cur_ptr;
                ++cur_line_symbol;

                *t_token_type = (YelTokenType)tok_string;

                goto _end_;
            }
            else if (cur_char == '\n') {
                yel_print_error("LexicalError", "a new line when reading a string literal", source, cur_line, cur_line_symbol);
                *t_token_type = tok_undefined;
                return;
            }

            token_value[token_value_counter] = cur_src[cur_ptr];
            ++token_value_counter;
            ++cur_ptr;
            ++cur_line_symbol;
        }

        printf("LexicalError: module %s\n-->  %lu:%lu: unclosed string \n|\n|", 
                source->file_name, com_start[0], com_start[1]);

        *t_token_type = tok_undefined;
        return;

        goto _end_;
    }

    start_symbol = cur_line_symbol;
    // binary operators
    switch (cur_char)
    {
    case '%':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_percent_assign;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_percent;
            goto _count_bin_operator1;
        }
    case '=':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_eq;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_assign;
            goto _count_bin_operator1;
        }

    case '+':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_plus_assign;
            goto _count_bin_operator2;
        } else if (cur_src[cur_ptr+1] == '+') {
            *t_token_type = (YelTokenType)tok_unary_op_inc;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_plus;
            goto _count_bin_operator1;
        }

    case '-':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_minus_assign;
            goto _count_bin_operator2;
        } else if (cur_src[cur_ptr+1] == '-') {
            *t_token_type = (YelTokenType)tok_unary_op_dec;
            goto _count_bin_operator2;
        } else if (cur_src[cur_ptr+1] == '>') {
            *t_token_type = (YelTokenType)tok_op_follow;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_minus;
            goto _count_bin_operator1;
        }

    case '*':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_mul_assign;
            goto _count_bin_operator2;
        } else if (cur_src[cur_ptr+1] == '*') {
            if (cur_src[cur_ptr+2] == '=') {
                *t_token_type = (YelTokenType)tok_binary_op_pow_assign;
                goto _count_bin_operator3;
            } else {
                *t_token_type = (YelTokenType)tok_binary_op_pow;
                goto _count_bin_operator2;
            }
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_mul;
            goto _count_bin_operator1;
        }

    case '/':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_div_assign;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_div;
            goto _count_bin_operator1;
        }

    case '>':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_more_eq;
            goto _count_bin_operator2;
        } else if(cur_src[cur_ptr+1] == '>') {
            if (cur_src[cur_ptr+2] == '=') {
                *t_token_type = (YelTokenType)tok_binary_op_rsh_assign;
                goto _count_bin_operator3;
            } else {
                *t_token_type = (YelTokenType)tok_binary_op_rsh;
                goto _count_bin_operator2;
            }
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_more;
            goto _count_bin_operator1;
        }

    case '<':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_less_eq;
            goto _count_bin_operator2;
        } else if(cur_src[cur_ptr+1] == '<') {
            if (cur_src[cur_ptr+2] == '=') {
                *t_token_type = (YelTokenType)tok_binary_op_lsh_assign;
                goto _count_bin_operator3;
            } else {
                *t_token_type = (YelTokenType)tok_binary_op_lsh;
                goto _count_bin_operator2;
            }
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_less;
            goto _count_bin_operator1;
        }

    case '&':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_and_assign;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_and;
            goto _count_bin_operator1;
        }

    case '^':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_xor_assign;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_xor;
            goto _count_bin_operator1;
        }

    case '|':
        if (cur_src[cur_ptr+1] == '=') {
            *t_token_type = (YelTokenType)tok_binary_op_or_assign;
            goto _count_bin_operator2;
        } else {
            *t_token_type = (YelTokenType)tok_binary_op_or;
            goto _count_bin_operator1;
        } 
        
    case '~':
        *t_token_type = (YelTokenType)tok_unary_op_not;
        goto _count_bin_operator1;
    }

goto _op;
_count_bin_operator1:
    token_value[token_value_counter] = cur_src[cur_ptr];
    ++token_value_counter;
    ++cur_ptr;
    ++cur_line_symbol;
    goto _end_;

_count_bin_operator2:
    token_value[token_value_counter] = cur_src[cur_ptr];
    token_value[++token_value_counter] = cur_src[++cur_ptr];
    ++token_value_counter;
    cur_ptr += 1;
    cur_line_symbol += 3;
    goto _end_;

_count_bin_operator3:
    token_value[token_value_counter] = cur_src[cur_ptr];
    token_value[++token_value_counter] = cur_src[++cur_ptr];
    token_value[++token_value_counter] = cur_src[++cur_ptr];
    ++token_value_counter;
    ++cur_ptr;
    cur_line_symbol += 4;
    goto _end_;

_op:
    start_symbol = cur_line_symbol;
    switch (cur_char) {
    case '(': *t_token_type = (YelTokenType)tok_op_lpar;
        break;
    case ')': *t_token_type = (YelTokenType)tok_op_rpar;
        break;
    case '{': *t_token_type = (YelTokenType)tok_op_flbrk;
        ++f_brk; 
        break;
    case '}':
        if (f_brk == 0) {
            yel_print_error("SyntaxError", "invalid syntax, expected '{'", 
                source, cur_line, cur_line_symbol);
            *t_token_type = tok_undefined;
            goto _end_;
        }
        *t_token_type = (YelTokenType)tok_op_frbrk;
        --f_brk;
        break;
    case ';': *t_token_type = (YelTokenType)tok_semicolon;
        break;
    case ':': *t_token_type = (YelTokenType)tok_colon;
        break;
    case ',': *t_token_type = (YelTokenType)tok_comma;
        break; 
    case ' ': case '\n': case '\0':
        ++cur_ptr;
        ++cur_line_symbol;
        goto _start_;
    default: 
        yel_print_error("SyntaxError", "invalid syntax", 
            source, cur_line, cur_line_symbol);
        *t_token_type = tok_undefined;
        goto _end_;
    }
    
    token_value[token_value_counter] = cur_src[cur_ptr];
    ++token_value_counter;
    ++cur_ptr;
    ++cur_line_symbol;

_end_:
    while (token_value[token_value_counter] != '\0') 
        token_value[token_value_counter++] = '\0';

    if (*t_token_type == (YelTokenType)tok_name) {
        for (int i = 0; i < yel_keywords_length; i++) {
           if (__builtin_strcmp(yel_keywords[i], token_value) == 0) {
                *t_token_type = (YelTokenType)((int)tok_word+i+1);
                break;
            }
        }

        if (__builtin_strcmp("and", token_value) == 0) {
            *t_token_type = tok_binary_op_log_and;
        }
        else if (__builtin_strcmp("or", token_value) == 0) {
            *t_token_type = tok_binary_op_log_or;
        }
        else if (__builtin_strcmp("not", token_value) == 0) {
            *t_token_type = tok_unary_op_log_not;
        }
        else if (__builtin_strcmp("True", token_value) == 0) {
            *t_token_type = tok_bool_true;
        }
        else if (__builtin_strcmp("False", token_value) == 0) {
            *t_token_type = tok_bool_false;
        }
    }
}

YelTokens yel_parse_tokens(YelSource* source) {
    YelTokens yel_tokens = { 0 };
    yel_tokens.pointer = 0;
    yel_tokens.file_name = source->file_name;
    yel_tokens.error = 0;
    yel_tokens.src_ptr = source;

    YelTokenType token_type;

    char token_value[2048];
    
    yel_tokens.type = (YelTokenType*)__builtin_malloc((size_t)(sizeof(YelTokenType)));
    if (yel_tokens.type = NULL) {
        printf("MemoryAllocationError <module Lexer>\n--> Heap was corrupted. Unable to allocate memory to buffer.\n|\n|");
        return yel_tokens;
    }

    yel_tokens.value = (char**)__builtin_malloc((size_t)(sizeof(char*)));
    if (yel_tokens.value = NULL) {
        printf("MemoryAllocationError <module Lexer>\n--> Heap was corrupted. Unable to allocate memory to buffer.\n|\n|");
        return yel_tokens;
    }

    yel_tokens.start_symbol = (size_t*)__builtin_malloc((size_t)(sizeof(size_t)));
    if (yel_tokens.start_symbol = NULL) {
        printf("MemoryAllocationError <module Lexer>\n--> Heap was corrupted. Unable to allocate memory to buffer.\n|\n|");
        return yel_tokens;
    }

    yel_tokens.line = (size_t*)__builtin_malloc((size_t)(sizeof(size_t)));
    if (yel_tokens.line = NULL) {
        printf("MemoryAllocationError <module Lexer>\n--> Heap was corrupted. Unable to allocate memory to buffer.\n|\n|");
        return yel_tokens;
    }

    yel_tokens.length = 0;
    size_t tmp_malloc = 0;

    while (source->pointer < source->length) {
        if (yel_tokens.length == tmp_malloc) {
            tmp_malloc += 64UL;

            yel_tokens.type = (YelTokenType*)__builtin_realloc(yel_tokens.type, (size_t)((tmp_malloc) * sizeof(YelTokenType)));
            yel_tokens.value = (char**)__builtin_realloc(yel_tokens.value, (size_t)((tmp_malloc) * sizeof(char*)));
            yel_tokens.start_symbol = (size_t*)__builtin_realloc(yel_tokens.start_symbol, (size_t)((tmp_malloc) * sizeof(size_t)));
            yel_tokens.line = (size_t*)__builtin_realloc(yel_tokens.line, (size_t)((tmp_malloc) * sizeof(size_t)));
        }

        yel_tokens.type[yel_tokens.length] = tok_semicolon;

        yel_get_next_token(
            source, &yel_tokens.type[yel_tokens.length], token_value
        );

        if (yel_tokens.type[yel_tokens.length] == tok_undefined) {
            yel_tokens.error = 1;
            return yel_tokens;
        }

        yel_tokens.line[yel_tokens.length] = cur_line;
        yel_tokens.start_symbol[yel_tokens.length] = start_symbol;
        
        yel_tokens.value[yel_tokens.length] = (char*)__builtin_malloc((size_t)(__builtin_strlen(token_value) * sizeof(char)));
        if (yel_tokens.value[yel_tokens.length] == NULL) {
            printf("MemoryAllocationError <module Lexer>\n--> Heap was corrupted. Unable to allocate memory to buffer.\n|\n|");
            
            yel_tokens.error = 1;
            
            return yel_tokens;
        }

        size_t l = __builtin_strlen(token_value);
        for (size_t i = 0; i < l; ++i) {
            yel_tokens.value[yel_tokens.length][i] = token_value[i];
        }
        yel_tokens.value[yel_tokens.length][l] = '\0';
        
        ++yel_tokens.length;
    }

    if (f_brk != 0) {
        printf("File '%s'\n\nSyntaxError: unexpected end of file\n", source->file_name);
        yel_tokens.error = 1;
    }

    return yel_tokens;
}

void yel_free_tokens(YelTokens* yel_tokens) {
    while (yel_tokens->length) {
        __builtin_free(yel_tokens->value[--yel_tokens->length]);
    }
    
    __builtin_free(yel_tokens->line);
    __builtin_free(yel_tokens->start_symbol);
    __builtin_free(yel_tokens->value);
    __builtin_free(yel_tokens->type);
}
