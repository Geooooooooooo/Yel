#include "io.h"
#include "lexer.h"
#include "syntaxer.h"
#include "dependencies.h"

// TODO: unclosed block -> { ... and ...}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No input file\nUse: yel <filename>\n");
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

    //print_tokens();

    yel_gen_parse_tree(&token_array);

_yel_end:
    yel_free_tokens(&token_array);
    __builtin_free(source.source_text);

    return 0;
}