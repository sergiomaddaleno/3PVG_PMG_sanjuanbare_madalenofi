#ifndef UTILS
#define UTILS

#include <memory> 
#include <iostream>

char* ReadFile(const char* file_path=nullptr) {
    FILE* file = fopen(file_path, "rb");
    long size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* my_file = (char*)malloc(size + 1);
        if (my_file) {
            fread(my_file, sizeof(char), size, file);
            my_file[size] = '\0';

            printf("%s\n", my_file);
        }
        fclose(file);

        return my_file;
    }
    else {
        printf("Error in loading file\n");
    }

    return nullptr;
}

#endif // UTILS