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

enum Effect{
	Grey,
	Negative,
	Waves,
	Blur,
};

class PostProcessing {
public:

	void initData();

	void BeginRender();
	
	void EndRender();

	void Render(GLuint& program, Effect effect);

	// --
	void resize(unsigned int width, unsigned int height);

	void setupShader(GLuint& program, const char* vertex_src, const char* fragment_src);

	// --

	float grey_ = 0.0f;
	float negative_ = 1.0f;
	float waves_ = 0.0f;
	float blur_ = 0.0f;

private:

	unsigned int width_=800, height_=600;

	GLuint FBO=0, RBO=0, FBTexture=0;
	GLuint rectVAO=0, rectVBO=0;

	GLuint program_=0;

};

#endif // PP
