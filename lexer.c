#include "lexer.h"

#define cur_src     source->source_text
#define cur_ptr     source->pointer
#define cur_len     source->length
#define cur_char    cur_src[cur_ptr]

#define yel_is_op(c) (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ':')
#define yel_is_bin_op(c) (c == '*' || c == '+' || c == '-' || c == '/' || c == '>' || c == '<' || c == '=' || c == ',')
#define yel_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
#define yel_is_number(c) (c >= '0' && c <= '9')

const int yel_keywords_length = 8;
const const char* yel_keywords[] = {
    "return", "defer", "func", 
    "noreturn", "int", "float", "string", "any",
};

void yel_get_next_token(Source* source, YelTokenType* t_token_type, char* token_value) {
    register size_t token_value_counter = 0;

_start_:
    while ((cur_ptr < cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;

    // comments
    if (cur_char == '/' && cur_src[cur_ptr + 1] == '/') {
        while ((cur_ptr < cur_len) && cur_char != '\n') 
            ++cur_ptr;

        goto _start_;
    }
    else if (cur_char == '/' && cur_src[cur_ptr + 1] == '*') {
        _Bool close_comment = 0;

        while (cur_ptr < cur_len) {
            if (cur_char == '*' && cur_src[cur_ptr + 1] == '/') {
                close_comment = 1;
                cur_ptr += 2;
                
                goto _start_;
            }

            ++cur_ptr;
        }

        if (!close_comment) {
            printf("LexerError: unclosed comment\n");
            goto _end_;
        }
    }

    // num
    if (yel_is_number(cur_char) || cur_char == '.') {
        _Bool dot = ((cur_char == '.') ? (
            1, token_value[token_value_counter++] = cur_src[cur_ptr++]
        ) : 0);

        while (1) {
            if (cur_char == '.') {
                if (dot) {
                    printf("LexerError '%c'\n", cur_char);
                    break;
                }

                token_value[token_value_counter++] = cur_char;
                dot = 1;
            }
            else if(yel_is_number(cur_char)) {
                token_value[token_value_counter++] = cur_char;
            }
            else {
                if (cur_char == ' ' || yel_is_bin_op(cur_char) || yel_is_op(cur_char)) {
                    break;
                }
            }

            ++cur_ptr;

        }
        *t_token_type = (YelTokenType)tok_number;

        goto _end_;
    }

    // var
    if (yel_is_alpha(cur_char)) {
        while (1) {
            if (yel_is_alpha(cur_char)) {
                token_value[token_value_counter++] = cur_char;
            }
            else if (cur_char == ' ' || yel_is_bin_op(cur_char) || yel_is_op(cur_char)) {
                break;
            }

            ++cur_ptr;

        }

        *t_token_type = (YelTokenType)tok_name;

        goto _end_;
    }

    // string
    if (cur_char == '\'' || cur_char == '\"') {
        char b = cur_char;
        ++cur_ptr;

        while (cur_ptr <= cur_len) {
            if (cur_char == b) {
                ++cur_ptr;

                *t_token_type = (YelTokenType)tok_string;

                goto _end_;
            }
            else if (cur_char == '\n') {
                printf("LexerError '%c'\n", cur_char);
            }

            token_value[token_value_counter++] = cur_src[cur_ptr++];
        }

        printf("LexerError unclosed string\n");

        goto _end_;
    }

    // binary operators
    if (yel_is_bin_op(cur_char)) {
        *t_token_type = (YelTokenType)tok_binary_op;

        if (cur_src[cur_ptr + 1] == '=') {
            token_value[token_value_counter++] = cur_src[cur_ptr];
            token_value[token_value_counter++] = cur_src[++cur_ptr];
            cur_ptr += 2;
        }
        else if (cur_src[cur_ptr] == '-' && cur_src[cur_ptr + 1] == '>') {
            token_value[token_value_counter++] = cur_src[cur_ptr];
            token_value[token_value_counter++] = cur_src[++cur_ptr];
            ++cur_ptr;
        }
        else {
            token_value[token_value_counter++] = cur_src[cur_ptr++];
        }

        goto _end_;
    }

    if (yel_is_op(cur_char)) {
        *t_token_type = (YelTokenType)tok_op;
        token_value[token_value_counter++] = cur_src[cur_ptr++];
    }

_end_:
    while (token_value[token_value_counter] != '\0') 
        token_value[token_value_counter++] = '\0';

    if (*t_token_type == (YelTokenType)tok_name) {
        for (int i = 0; i < yel_keywords_length; i++) {
            if (__builtin_strcmp(yel_keywords[i], token_value) == 0) {
                *t_token_type = (YelTokenType)tok_word;
                break;
            }
        }
    }

    while ((cur_ptr < cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;
}

YelTokens yel_parse_tokens(Source* source) {
    YelTokens yel_tokens = { 0 };
    yel_tokens.pointer = 0;
    YelTokenType token_type;

    char token_value[2048];
    
    yel_tokens.type = (YelTokenType*)__builtin_malloc((size_t)(sizeof(YelTokenType)));
    if (yel_tokens.type = NULL) {
        printf("MemoryAllocationError <module Lexer>\n\tHeap was corrupted. Unable to allocate memory to buffer.\n");
        return yel_tokens;
    }

    yel_tokens.value = (char**)__builtin_malloc((size_t)(sizeof(char*)));
    if (yel_tokens.value = NULL) {
        printf("MemoryAllocationError <module Lexer>\n\tHeap was corrupted. Unable to allocate memory to buffer.\n");
        return yel_tokens;
    }

    yel_tokens.length = 0;
    while (source->pointer < source->length) {
        yel_tokens.type = (YelTokenType*)__builtin_realloc(yel_tokens.type, (size_t)((yel_tokens.length + 2) * sizeof(YelTokenType)));
        yel_tokens.value = (char**)__builtin_realloc(yel_tokens.value, (size_t)((yel_tokens.length + 2) * sizeof(char*)));

        yel_get_next_token(source, &yel_tokens.type[yel_tokens.length], token_value);
        
        yel_tokens.value[yel_tokens.length] = (char*)__builtin_malloc((size_t)(__builtin_strlen(token_value) * sizeof(char)));
        if (yel_tokens.value[yel_tokens.length] == NULL) {
            printf("MemoryAllocationError <module Lexer>\n\tHeap was corrupted. Unable to allocate memory to buffer.\n");
            
            yel_free_tokens(&yel_tokens);
            
            return yel_tokens;
        }

        // strcpy ();
        size_t l = __builtin_strlen(token_value);
        for (size_t i = 0; i < l; i++) {
            yel_tokens.value[yel_tokens.length][i] = token_value[i];
        }
        yel_tokens.value[yel_tokens.length][l] = '\0';
        
        ++yel_tokens.length;
    }

    return yel_tokens;
}

void yel_free_tokens(YelTokens* yel_tokens) {
    while (yel_tokens->length) {
        __builtin_free(yel_tokens->value[yel_tokens->length--]);
    }
    
    __builtin_free(yel_tokens->value);
    __builtin_free(yel_tokens->type);
}
