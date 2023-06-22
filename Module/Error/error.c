#include "error.h"

void yel_print_error(const char* err_name, const char* err_desc, Source* src, size_t line, size_t symbol) {
    printf("File '%s', line %lu\n\n    ", src->file_name, line, symbol);

    size_t line_counter = 1;

    for (size_t i = 0; i < src->length; i++) {
        if (line_counter == line) {
            size_t symbol_counter = 1;
            size_t start = i;

            while (src->source_text[start] == ' ') {
                ++start;
                ++symbol_counter;
                ++i;
            }

            for (size_t l = start; l < src->length; l++) {
                if (src->source_text[l] == '\n') break;
                printf("%c", src->source_text[l]);
                if (src->source_text[l] == ';' && l >= symbol) break;
            }
            
            printf("\n    ");
            while (1) {
                if (src->source_text[i] == '\n') break;

                else if (symbol_counter == symbol) printf("^");
                else printf("~");

                if (src->source_text[i] == ';') break;

                ++i;
                ++symbol_counter;
            }

            break;
        }
        else if (src->source_text[i] == '\n') {
            ++line_counter;
        }
    }

    printf("\n\n%s: %s\n", err_name, err_desc);

}