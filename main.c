#include "io.h"
#include "lexer.h"
#include "syntaxer.h"
#include "dependencies.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No input file\nUse: yel <filename>\n");
        return -1;
    }

    Source source = yel_stdio_read_file(argv[1]);
    if (source.source_text == NULL) {
        return -1;
    }

    YelTokens token_array = yel_parse_tokens(&source);

    //for (size_t i = 0; i < token_array.length; i++) {
    //    printf("[%d, %s, %lu:%lu]\n", token_array.type[i], token_array.value[i], token_array.line[i], token_array.start_symbol[i]);
    //}

    yel_gen_parse_tree(&token_array);

    yel_free_tokens(&token_array);
    __builtin_free(source.source_text);

    return 0;
}