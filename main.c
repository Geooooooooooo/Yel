#include "d_io.h"
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

    /*puts("Lexer:\n");
    for(size_t i = 0; i < token_array.length; i++) {
        printf("[%d: '%s']\n", token_array.type[i], token_array.value[i]);
    }*/

    //puts("\nParser:\n");
    yel_gen_parse_tree(&token_array);

    yel_free_tokens(&token_array);
    yel_free_string(source.source_text);

    return 0;
}