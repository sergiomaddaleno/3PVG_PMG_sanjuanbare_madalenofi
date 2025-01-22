
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
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <cassert>

void CheckGPUErrors(unsigned int code, const char* str, bool link) {
    
    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::string errorMessage;
        switch (error) {
        case GL_INVALID_ENUM: errorMessage = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE: errorMessage = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION: errorMessage = "GL_INVALID_OPERATION"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errorMessage = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        case GL_OUT_OF_MEMORY: errorMessage = "GL_OUT_OF_MEMORY"; break;
        default: errorMessage = "Unknown OpenGL error"; break;
        }
        std::cerr << "[OpenGL Error] " << errorMessage << " at: " << str << std::endl;
    }

    // Check for shader or program errors
    int success = 0;
    char infoLog[512] = { 0 };

    if (link) {
        glGetProgramiv(code, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(code, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "[Program Link Error] " << infoLog << " at: " << str << std::endl;
        }
    }
    else {
        glGetShaderiv(code, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(code, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "[Shader Compile Error] " << infoLog << " at: " << str << std::endl;
        }
    }

    assert(error == GL_NO_ERROR && success);
}

