#ifndef UTILS
#define UTILS

#include <memory> 
#include <iostream>
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include "../deps/dds.h"
#include "../deps/glm/glm.hpp"

#include "camera.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"

struct RenderData {

	Camera cam;
	RenderComponent render_comp;
	TransformComponent transf_comp;

	int w_width = 800;
	int w_height = 600;
	float fov = 55.0f;

	bool b_fog_ = true;
};

char* ReadFile(const char* file_path = nullptr);

void CheckGPUErrors(unsigned int code, const char* str, bool link);



#endif // UTILS