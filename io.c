#include "d_io.h"
#include "dependencies.h"

Source daf_stdio_read_file(char* filename) {
    Source source;
    source.pointer = 0;
    
    FILE *file_p;
    if ((file_p = fopen(filename, "r")) == ((void*)0)) {
        printf("IStreamError <File '%s'>\nUnable to open the file\n", filename);
        return (Source){0, 0, 0};
    }

    fseek(file_p, 0L, SEEK_END);
    source.length = (size_t)ftell(file_p);
    fseek(file_p, 0L, SEEK_SET);

    source.source_text = (char*)malloc((size_t)((source.length + 2) * sizeof(char)));
    if (source.source_text == NULL) {
        printf("MemoryAllocationError <File '%s'>\nUnable to allocate memory to buffer.\n", filename);
        return (Source){0, 0, 0};
    }

    for (size_t i = 0; i < source.length; i++) {
        source.source_text[i] = fgetc(file_p);
    }
    source.source_text[source.length] = '\0';

    fclose(file_p);

    return source;
}

// free(void* __ptr)
void daf_free_string(char* string) {
    free(string);
}
