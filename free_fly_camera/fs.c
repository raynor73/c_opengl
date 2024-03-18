#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

char *load_text_file(const char *path) {
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        error("Error opening file %s\n", path);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = (char *) malloc(file_size + 1);
    if (buffer == NULL) {
        error("Memory allocation failed while reading file %s\n", path);
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    
    return buffer;
}
