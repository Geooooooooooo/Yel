#include "lexer.h"
#include "d_io.h"
#include "dependencies.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No input file\nUse:\n\tdaf <filename>\n");
        return -1;
    }

    Source source = daf_stdio_read_file(argv[1]);
    if (source.source_text == ((void*)0)) {
        return -1;
    }

    DafTokens token_array = daf_parse_tokens(&source);

    char tabs[10] = { '\0' };
    int c = 0;
    for(size_t i = 0; i < token_array.length; i++) {
        printf("[%d: '%s']", token_array.type[i], token_array.value[i]);
    }

    daf_free_tokens(&token_array);
    daf_free_string(source.source_text);

    return 0;
}
