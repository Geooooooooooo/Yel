#include "lexer.h"

#define cur_src     source->source_text
#define cur_ptr     source->pointer
#define cur_len     source->length
#define cur_char    cur_src[cur_ptr]

#define daf_is_operator(c) (c == '!' || (c >= '%' && c <= '/') || c == ':' || c <= '?' || c == '<' || c == '>' || c == '{' || c == '}' || c == '[' || c == ']')
#define daf_is_alpha(c) ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
#define daf_is_number(c) (c >= '0' && c <= '9')


void daf_get_next_token(Source* source, DafTokenType* t_token_type, char* token_value) {
    size_t token_value_counter = 0;

_start_:
    while ((cur_ptr <= cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;

    // comments
    if (cur_char == '/' && cur_src[cur_ptr + 1] == '/') {
        while ((cur_ptr <= cur_len) && cur_char != '\n') 
            ++cur_ptr;

        goto _start_;
    }
    else if (cur_char == '/' && cur_src[cur_ptr + 1] == '*') {
        _Bool close_comment = 0;

        while (cur_ptr <= cur_len) {
            if (cur_char == '*' && cur_src[cur_ptr + 1] == '/') {
                close_comment = 1;
                cur_ptr += 2;
                break;
            }

            ++cur_ptr;

            goto _start_;
        }

        if (!close_comment) {
            printf("LexerError: unclosed comment\n");
            goto _end_;
        }
    }

    // num
    if (daf_is_number(cur_char) || cur_char == '.') {
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
            else if(daf_is_number(cur_char)) {
                token_value[token_value_counter++] = cur_char;
            }
            else {
                if (cur_char == ' ' || daf_is_operator(cur_char)) {
                    break;
                }
                else {
                    printf("LexerError '%c'\n", cur_char);
                    break;
                }
            }

            ++cur_ptr;
        }
        *t_token_type = (DafTokenType)number;
        ++cur_ptr;

        goto _end_;
    }

    while ((cur_ptr <= cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;

    // var
    if (daf_is_alpha(cur_char)) {
        while (1) {
            if (daf_is_alpha(cur_char)) {
                token_value[token_value_counter++] = cur_char;
            }
            else if (cur_char == ' ' || daf_is_operator(cur_char)) {
                break;
            }

            ++cur_ptr;
        }

        *t_token_type = (DafTokenType)variable;

        goto _end_;
    }

    while ((cur_ptr <= cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;

    // string
    if (cur_char == '\'' || cur_char == '\"') {
        char b = cur_char;
        token_value[token_value_counter++] = cur_src[cur_ptr++];

        while (cur_ptr <= cur_len) {
            if (cur_char == '\\') {
                char next_char = cur_src[cur_ptr + 1];

                if (next_char == 'r')
                    token_value[token_value_counter++] = '\r';
                else if (next_char == 'n')
                    token_value[token_value_counter++] = '\n';
                else if (next_char == 't')
                    token_value[token_value_counter++] = '\t';
                else if (next_char == 'a')
                    token_value[token_value_counter++] = '\a';
                else if (next_char == 'v')
                    token_value[token_value_counter++] = '\v';
                else if (next_char == '\'')
                    token_value[token_value_counter++] = '\'';
                else if (next_char == '\\')
                    token_value[token_value_counter++] = '\\';
                else if (next_char == '?')
                    token_value[token_value_counter++] = '\?';
                else {
                    token_value[token_value_counter++] = '\\';
                    token_value[token_value_counter++] = next_char;
                }

                cur_ptr += 2;
                continue;
            }
            else if (cur_char == b) {
                token_value[token_value_counter++] = cur_src[cur_ptr++];

                *t_token_type = (DafTokenType)string;

                goto _end_;
            }
            else if (cur_char == '\n') {
                printf("LexerError '%c'\n", cur_char);
            }

            token_value[token_value_counter++] = cur_src[cur_ptr++];
        }

        printf("LexerError unclosed string\n");
    }

    while ((cur_ptr <= cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;

    // operators
    if (daf_is_operator(cur_char)) {
        *t_token_type = (DafTokenType)operator;

        if ((cur_char == '<' || cur_char == '>') && (cur_src[cur_ptr + 1] == '=')) {
            token_value[token_value_counter++] = cur_src[cur_ptr++];
            token_value[token_value_counter++] = cur_src[cur_ptr];
            cur_ptr += 2;
            source->pointer++;

            goto _end_;
        }

        token_value[token_value_counter++] = cur_src[cur_ptr++];
    }

_end_:
    while (token_value[token_value_counter] != '\0') 
        token_value[token_value_counter++] = '\0';

    while ((cur_ptr <= cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        ++cur_ptr;
}
