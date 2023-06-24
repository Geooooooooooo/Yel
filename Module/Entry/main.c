#include "../IO/io.h"
#include "../Lexer/lexer.h"
#include "../Parser/syntaxer.h"
#include "../Parser/parser.h"
#include "../Dependencies/dependencies.h"

#include <stdlib.h>

// TODO: create checker to (-q -= (1 - 2) ** 3);
//                             ^^
//                             invalid syntax

// IN FUTURE: Instead of each bin_op use '''bin_op 10'''

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No input file\nUse: 'yel <filename>'\n");
        return -1;
    }

    Source source = yel_stdio_read_file(argv[1]);
    if (source.source_text == NULL) {
        return 0;
    }

    YelTokens token_array = yel_parse_tokens(&source);
    if (token_array.error) {
        goto _yel_end;
    }

    OPCODES codes;
    yel_gen_opcode(&token_array, &codes);

    for (size_t i = 0; i < codes.len; i++) {
        printf("%llu\n", codes.codes[i]);
    }

_yel_end:
    yel_free_tokens(&token_array);
    __builtin_free(source.source_text);

    return 0;
}