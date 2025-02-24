
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

void CheckGPUErrors(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << "OpenGL Error (" << error << ") | " << file << " (" << line << ")" << std::endl;
    }
}

void CheckShaderErrors(GLuint shader, const char* shaderType)
{
    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        assert(false);
    }
}

void CheckProgramErrors(GLuint program) {
    GLint success;
    GLchar infoLog[1024];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        assert(false);
    }
}


