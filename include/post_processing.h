#ifndef PP
#define PP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

class PostProcessing {
public:

	void initData();

	void BeginRender();
	
	void EndRender();

	void Render();

	// --
	void resize(unsigned int width, unsigned int height);

	void setupShader(const char* vertex_src, const char* fragment_src);

	// --


	GLuint FBO, RBO, FBTexture;

	float grey_value_ = 0.0f;

private:

	unsigned int width_=800, height_=600;

	GLuint rectVAO, rectVBO;

	GLuint program_;

};

#endif // PP
