#ifndef UTILS
#define UTILS

#include <memory> 
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include "../deps/dds.h"
#include "../deps/glm/glm.hpp"

#include "Render/camera.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"

struct RenderData {

	Camera cam;
	RenderComponent render_comp;
	TransformComponent transf_comp;

	int w_width = 800;
	int w_height = 600;
	float fov = 55.0f;

	bool b_fog_ = false;
};

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