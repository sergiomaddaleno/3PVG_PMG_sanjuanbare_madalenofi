
#ifndef __CUBEMAP__
#define __CUBEMAP__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>
#include "Core/texture.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Cubemap {

public:

	Cubemap();
	~Cubemap();

	void init();
	void bind();

private:

	GLuint TextureID;
	int width, height, channels;
	unsigned char* data_;

	std::vector<std::string> textures_faces;
};

#endif // __CUBEMAP__