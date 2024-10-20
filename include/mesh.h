
#ifndef MSL
#define MSL

#include "../deps/linmath.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <vector>

struct Vertex {
	vec3 pos;
	vec3 normal;
	vec2 tex_coord;
};

class Mesh {

public:

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh() {

	}

	~Mesh() {

	}
	
	bool LoadOBj(const std::string& file);

};



#endif // !MSL
