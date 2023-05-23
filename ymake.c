#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <dirent.h>

char* yel_stdio_read_file(char* filename);

inline void ymake_init() {

}

inline int ymake_compile() {
    char current_dir[1024];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
       perror("Error: Unable to get the current directory\n");
       return 1;
    }

    printf("Assembly for %s started\n", current_dir);

    DIR* d;
    struct dirent *dir;

    d = opendir(current_dir);
    if (!d) {
        printf("Unable to open the /tmp/ dir\n");
        return 0;
    }

    while ((dir = readdir(d)) != NULL) {
        printf("%s\n", dir->d_name);
    }

    closedir(d);
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        if (__builtin_strcmp(argv[0], "init") == 0) {
            ymake_init();
        }
        else {
            return ymake_compile();
        }
    } 
    else {
        // error
    }   
}

char* yel_stdio_read_file(char* filename) {
    char* source;
    
    FILE *file_p;
    if ((file_p = fopen(filename, "r")) == NULL) {
        printf("IStreamError <File '%s'>\nUnable to open the file\n", filename);
        return (char*)(0);
    }

    fseek(file_p, 0L, SEEK_END);
    size_t length = (size_t)ftell(file_p);
    fseek(file_p, 0L, SEEK_SET);

    source = (char*)__builtin_malloc((size_t)((length + 2) * sizeof(char)));
    if (source == NULL) {
        printf("MemoryAllocationError <File '%s'>\nUnable to allocate memory to buffer.\n", filename);
        return (char*)(0);
    }

    for (size_t i = 0; i < length; i++) {
        source[i] = fgetc(file_p);
    }
    source[length] = '\0';

    fclose(file_p);

    return source;
}