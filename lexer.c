#include "lexer.h"

#define cur_src     source->source_text
#define cur_ptr     source->pointer
#define cur_len     source->length
#define cur_char    cur_src[cur_ptr]


DafToken daf_get_next_token(Source* source) {
    DafToken token;
    token.token_value = ((void*)0);

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) cur_ptr++;

    // num
    if (daf_is_number(cur_char) || cur_char == '.') {
        _Bool dot = ((cur_char == '.') ? (
            1, printf("%c", cur_char), cur_ptr++
        ) : 0);

        while (1) {
            if (cur_char == '.') {
                if (dot) {
                    printf("LexerError '%c'\n", cur_char);
                    break;
                }

                printf("%c", cur_char);
                dot = 1;
            }
            else if(daf_is_number(cur_char)) {
                printf("%c", cur_char);
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
        token.token_type = (DafTokenType)number;
        source->pointer++;

        printf(" : num\n");

        return token;
    }

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) cur_ptr++;

    // var
    if (daf_is_alpha(cur_char)) {
        while (1) {
            if (daf_is_alpha(cur_char)) {
                printf("%c", cur_char);
            }
            else if (cur_char == ' ' || daf_is_operator(cur_char)) {
                break;
            }

            source->pointer++;
        }

        token.token_type = (DafTokenType)variable;
        printf(" : name\n");

        return token;
    }

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) cur_ptr++;

    // string
    if (cur_char == '\'' || cur_char == '\"') {
        char b = cur_char;
        cur_ptr++;

        printf("'");

        while (cur_ptr != cur_len) {
            if (cur_char == b) {
                printf("' : string\n");
                cur_ptr++;

                token.token_type = (DafTokenType)string;
                return token;
            }

            if (cur_char == '\n') {
                printf("LexerError '%c'\n", cur_char);
            }

            printf("%c", cur_char);
            
            cur_ptr++;
        }

        printf("LexerError unclosed string\n");
    }

    while ((cur_ptr != cur_len) && (cur_char == ' ' || cur_char == '\n')) cur_ptr++;

    // operators
    if (daf_is_operator(cur_char)) {
        if (cur_char == '<' || cur_char == '>') {
            if (cur_src[cur_ptr + 1] == '=') {
                printf("%c%c : operator\n", cur_char, cur_src[cur_ptr + 1]);
                source->pointer += 2;

                token.token_type = (DafTokenType)operator;

                return token;
            }
        }

        printf("%c: operator\n", cur_char);
        source->pointer++;

        return token;
    }

    return token;
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
