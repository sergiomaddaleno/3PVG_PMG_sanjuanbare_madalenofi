
#ifndef PRR
#define PRR

#include <optional>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/linmath.h>
#include "triangle.h"
#include "mesh.h"

class Render {

public:
	 
	GLuint program_;
	GLint mvp_location_;

	mat4x4 model, view, projection;

	GLuint VAO, VBO, EBO;

	Render();

	static std::optional<Render> make();

	GLuint GetProgram();
	GLint GetLocation();

	void RenderDrawTriangle(static const char* vertex_shader_text, static const char* fragment_shader_text,
		Triangle::PointsTriangle(&vertices)[3]);

	void RenderMesh(static const char* vertex_shader_text, static const char* fragment_shader_text, Mesh& m);

};

#endif // PRT