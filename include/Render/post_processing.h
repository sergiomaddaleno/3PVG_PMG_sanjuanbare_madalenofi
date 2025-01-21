#ifndef PP
#define PP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"

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

	GLuint getGamefBO() const {
		return FBTexture;
	}

	float grey_ = 0.0f;
	float negative_ = 1.0f;
	float waves_ = 0.0f;
	float blur_ = 0.0f;
	int kernel_types_ = 0;

private:

	unsigned int width_=800, height_=600;

	GLuint FBO=0, RBO=0, FBTexture=0;
	GLuint rectVAO=0, rectVBO=0;

};

#endif // PP
