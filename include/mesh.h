
#ifndef MSL
#define MSL

#include <GLFW/glfw3.h>
#include "../deps/linmath.h"
#include <string>
#include <vector>

struct Vertex {
	vec3 pos;
	vec3 normal;
	vec2 tex_coord;
};

class Mesh {

	Mesh() {

	}

	~Mesh() {

	}

	//--
	

	void LoadOBj(const char* file);


};



#endif // !MSL
