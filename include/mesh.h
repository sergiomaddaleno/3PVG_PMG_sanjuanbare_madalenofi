
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

	Mesh(Mesh&& other) noexcept
		: vertices(std::move(other.vertices)), indices(std::move(other.indices)) {
	}

	// Operador de asignación de movimiento
	Mesh& operator=(Mesh&& other) noexcept {
		if (this != &other) {
			vertices = std::move(other.vertices);
			indices = std::move(other.indices);
		}
		return *this;
	}
	
	bool LoadOBj(const std::string& file);

};



#endif // !MSL
