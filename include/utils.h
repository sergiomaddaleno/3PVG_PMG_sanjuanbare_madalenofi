#ifndef UTILS
#define UTILS

#include <memory> 
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include "../deps/dds.h"
#include "../deps/glm/glm.hpp"

char* ReadFile(const char* file_path = nullptr);

void CheckGPUErrors(unsigned int code, const char* str, bool link);

#endif // UTILS