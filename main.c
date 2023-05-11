#include "lexer.h"
#include "d_io.h"
#include "dependencies.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No input file");
        return -1;
    }

    Source source = daf_stdio_read_file(argv[1]);
    if (source.source_text == ((void*)0)) {
        return -1;
    }

    DafTokenType token_type;
    char token_value[2048];

    while (source.pointer != source.length) {
        daf_get_next_token(&source, &token_type, token_value);

        printf("%d: %s\n", token_type, token_value);
    }

    daf_free_string(source.source_text);

    return 0;
}
