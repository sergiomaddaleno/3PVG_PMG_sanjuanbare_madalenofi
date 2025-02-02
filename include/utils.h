#ifndef UTILS
#define UTILS

#include <memory> 
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include "../deps/dds.h"
#include "../deps/glm/glm.hpp"

enum Type {
    T_NONE = 0x0,
    T_FLOAT_1,
    T_FLOAT_2,
    T_FLOAT_3,
    T_FLOAT_4,

    T_INT_1,
    T_INT_2,
    T_INT_3,
    T_INT_4,

    T_MAT_4x4,
    T_MAT_3x3,
    T_MAT_2x2,

    T_SAMPLER_1D,
    T_SAMPLER_2D,
    T_SAMPLER_3D,
};

char* ReadFile(const char* file_path = nullptr);

void CheckGPUErrors(unsigned int code, const char* str, bool link);

#endif // UTILS