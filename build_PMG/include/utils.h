#ifndef __UTILS__
#define __UTILS__

#include <memory> 
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

char* ReadFile(const char* file_path = nullptr);

// usage example : CheckGPUErrors(__FILE__, __LINE__);
void CheckGPUErrors(const char* file, int line);

void CheckShaderErrors(GLuint shader, const char* shaderType);

void CheckProgramErrors(GLuint program);

#endif // __UTILS__