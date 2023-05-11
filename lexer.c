#include "lexer.h"

#define cur_src     source->source_text
#define cur_ptr     source->pointer
#define cur_len     source->length
#define cur_char    cur_src[cur_ptr]


void daf_get_next_token(Source* source, DafTokenType* t_token_type, char* token_value) {
    size_t token_value_counter = 0;

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) cur_ptr++;

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

            cur_ptr++;
        }
        *t_token_type = (DafTokenType)number;
        source->pointer++;
        
        while (token_value[token_value_counter] != '\0') 
            token_value[token_value_counter++] = '\0';

        return;
    }

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        cur_ptr++;

    // var
    if (daf_is_alpha(cur_char)) {
        while (1) {
            if (daf_is_alpha(cur_char)) {
                token_value[token_value_counter++] = cur_char;
            }
            else if (cur_char == ' ' || daf_is_operator(cur_char)) {
                break;
            }

            source->pointer++;
        }

        *t_token_type = (DafTokenType)variable;

        while (token_value[token_value_counter] != '\0') 
            token_value[token_value_counter++] = '\0';

        return;
    }

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) cur_ptr++;

    // string
    if (cur_char == '\'' || cur_char == '\"') {
        char b = cur_char;
        token_value[token_value_counter++] = cur_src[cur_ptr++];

        while (cur_ptr != cur_len) {
            if (cur_char == b) {
                token_value[token_value_counter++] = cur_src[cur_ptr++];

                *t_token_type = (DafTokenType)string;

                while (token_value[token_value_counter] != '\0') 
                    token_value[token_value_counter++] = '\0';
                return;
            }

            if (cur_char == '\n') {
                printf("LexerError '%c'\n", cur_char);
            }

            token_value[token_value_counter++] = cur_src[cur_ptr++];
        }

        printf("LexerError unclosed string\n");
    }

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) 
        cur_ptr++;

    // operators
    if (daf_is_operator(cur_char)) {
        *t_token_type = (DafTokenType)operator;

        if (cur_char == '<' || cur_char == '>') {
            if (cur_src[cur_ptr + 1] == '=') {
                token_value[token_value_counter++] = cur_src[cur_ptr++];
                token_value[token_value_counter++] = cur_src[cur_ptr];
                source->pointer += 2;

                while (token_value[token_value_counter] != '\0')
                    token_value[token_value_counter++] = '\0';
                return;
            }
        }

        token_value[token_value_counter++] = cur_char;
        source->pointer++;

        while (token_value[token_value_counter] != '\0') 
            token_value[token_value_counter++] = '\0';
    }
}

inline _Bool daf_is_number(char c) {
    return (c >= '0' && c <= '9');
}

inline _Bool daf_is_alpha(char c) {
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_');
}

inline _Bool daf_is_operator(char c) {
    return (c == '!' || (c >= '%' && c <= '/') || c == ':' || c <= '?' || c == '<' || c == '>' || c == '{' || c == '}' );
}
