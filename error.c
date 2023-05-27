#include "error.h"

void yel_print_error(Source* src, size_t line, size_t symbol) {
    --symbol;
    size_t line_counter = 1;
    size_t symbol_counter = 1;

    for (size_t i = 0; i < src->length; i++) {
        if (line_counter == line) {
            size_t i_start = i;
            size_t i_end = i;

            while (1) {
                if ((src->source_text[i_end] == ';' && symbol_counter >= symbol) || src->source_text[i] == '\n' ) {
                    ++i_end;
                    break;
                }
                else if (src->source_text[i_end] == ';') {
                    i_start = i_end + 1;
                }

                ++i_end;
                ++symbol_counter;
            }

            printf("    ");
            for (size_t l = i_start; l < src->length; l++) {
                if (src->source_text[l] == '\n') {
                    break;
                }

                printf("%c", src->source_text[l]);

                if (src->source_text[l] == ';') {
                    break;
                }
            }

            i_end -= i;

            printf("\n|    ");
            for (size_t l = i_start-i; l < i_end; l++) {
                if (l == symbol) {
                    printf("^");
                }
                else printf("~");
            }

            printf("\n|");

            size_t bottom = 25 + strlen(src->file_name);
            while (bottom) {
                printf("-");
                --bottom;
            }

            puts("\n");

            return;
        }

        if (src->source_text[i] == '\n') {
            ++line_counter;
            symbol_counter = 1;
        }

        ++symbol_counter;
    }
}