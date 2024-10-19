
#ifndef MSL
#define MSL
#include "../deps/tiny_obj_loader.h"
#include "../deps/linmath.h"
#include <string>
#include <iostream>
#include <vector>

struct Vertex {
	vec3 pos;
	vec3 normal;
	vec3 tex_coord;
};



class Mesh {

public:

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Mesh() {

	}

	~Mesh() {

	}

	

	//--
	

	bool LoadOBj(const std::string& file);


};



#endif // !MSL
