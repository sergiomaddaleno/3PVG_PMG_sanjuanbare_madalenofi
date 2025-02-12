#ifndef __POST_PROCESS__
#define __POST_PROCESS__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/shader.h"
#include "Core/framebuffer.h"

#include <string>
#include <iostream>
#include <vector>
#include <../deps/glm/glm.hpp>

static float rect_screen_vertices[] = {
	// positions   // texCoords
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,

	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

enum Effect{
	Grey,
	Negative,
	Waves,
	Blur,
	Kernel,
	None,
};

class PostProcessing {
public:

	PostProcessing();
	~PostProcessing();

	void initData();

	void beginRender();
	
	void endRender();

	void render(Shader& shader, Effect effect);

	// --

	void resize(unsigned int width, unsigned int height);

	float grey_;
	float negative_;
	float waves_;
	float blur_;
	int kernel_types_;

	FrameBuffer frame_buffer_;
private:

	unsigned int width_, height_;

	GLuint rectVAO, rectVBO, RBO;
};

#endif // __POST_PROCESS__
