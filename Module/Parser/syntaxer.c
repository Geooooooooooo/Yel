#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Lexer/lexer.h"
#include "../Errors/error.h"

#define curtype yel_tokens->type[yel_tokens->pointer]
#define nexttype yel_tokens->type[yel_tokens->pointer+1]
#define next yel_tokens->value[yel_tokens->pointer+1]
#define prev yel_tokens->value[yel_tokens->pointer-1]
#define cur yel_tokens->value[yel_tokens->pointer]

YelTokenType types[64];
int size_types = -1;

YelEntities yel_define_next_entity(YelTokens* yel_tokens) {
    size_types = -1;

    while (yel_tokens->pointer < yel_tokens->length) {
        if (curtype == tok_name) {
            if (size_types == -1) {
                

            }
        }

        ++yel_tokens->pointer;
    }
}

void yel_gen_parse_tree(YelTokens* yel_tokens) {
    while (yel_tokens->pointer < yel_tokens->length) {
        printf("%d\n", yel_define_next_entity(yel_tokens));
    }
}