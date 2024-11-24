
#ifndef PRR
#define PRR

#include <optional>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/linmath.h>
#include "triangle.h"
#include "mesh.h"

#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

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

	void SetupShader(const char* vertex_shader_text, const char* fragment_shader_text);

	void RenderDrawTriangle(Triangle::PointsTriangle(&vertices)[3], TransformComponent tr);

	void RenderMesh(std::unique_ptr<Mesh>& mesh, TransformComponent tr);

	void RenderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc);

};

#endif // PRT