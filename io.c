#include "io.h"
#include "dependencies.h"

Source yel_stdio_read_file(char* filename) {
    Source source;
    source.pointer = 0;

    source.file_name = (char*)malloc((size_t)(strlen(filename) * sizeof(char)));
    if (source.file_name == NULL) {
        printf("MemoryAllocationError <File '%s'>\nUnable to allocate memory to buffer.\n", filename);
        return (Source){0, 0, 0, 0};
    }
    
    FILE *file_p;
    if ((file_p = fopen(filename, "r")) == NULL) {
        printf("IStreamError <File '%s'>\nUnable to open the file\n", filename);
        return (Source){0, 0, 0, 0};
    }

    fseek(file_p, 0L, SEEK_END);
    source.length = (size_t)ftell(file_p);
    fseek(file_p, 0L, SEEK_SET);

    source.source_text = (char*)__builtin_malloc((size_t)((source.length + 2) * sizeof(char)));
    if (source.source_text == NULL) {
        printf("MemoryAllocationError <File '%s'>\nUnable to allocate memory to buffer.\n", filename);
        return (Source){0, 0, 0, 0};
    }
    
    for (size_t i = 0; i < source.length; i++) {
        source.source_text[i] = fgetc(file_p);
    }
    source.source_text[source.length] = '\0';

    fclose(file_p);

    //__builtin_strcpy(source.file_name, filename);
    size_t l = __builtin_strlen(filename);
    for (size_t i = 0; i < l; i++) {
        source.file_name[i] = filename[i];
    } source.file_name[l] = '\0';

    return source;
}

// __builtin_free(void* __ptr)
void yel_free_string(char* string) {
    __builtin_free(string);
}
