
#include <memory> 
#include <iostream>
#include <cassert>
#include "vector"

#include "utils.h"

char* ReadFile(const char* file_path) {

    FILE* file;
    if (fopen_s(&file, file_path, "rb") != 0) {
        printf("Error in loading file\n");
        return nullptr;
    }
        
    long size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* my_file = (char*)malloc(size + 1);
        if (my_file) {

            if (size <= 0) return nullptr;
            fread(my_file, sizeof(char), size, file);
            
            my_file[size] = '\0';

            //printf("%s\n", my_file);
        }
        fclose(file);

        return my_file;
    }

    return nullptr;
}

static char infoLog[512];
void CheckGPUErrors(unsigned int code, const char* str, bool link)
{
    int success = 0;
    if (link) {
        glGetProgramiv(code, GL_LINK_STATUS, &success);
        if (!success)
            glGetProgramInfoLog(code, 512, NULL, infoLog);
    }
    else {
        glGetShaderiv(code, GL_COMPILE_STATUS, &success);
        if (!success)
            glGetShaderInfoLog(code, 512, NULL, infoLog);
    }
    if (!success) {
        std::cout << str << infoLog << std::endl;
    }
    assert(success);
}
