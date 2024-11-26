
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

struct ShaderBuffers {
	char* vs = nullptr;
	char* fs = nullptr;
};

class Render {

public:
	
	Render();

	static std::optional<Render> make();

	// ---
	ShaderBuffers initShader(const char* vertex_shader_path, const char* fragment_shader_path,
		ShaderBuffers &shaders);

	void SetupShader(ShaderBuffers &shaders);
	// ---

	void RenderDrawTriangle(Triangle::PointsTriangle(&vertices)[3], TransformComponent tr);

	void RenderMesh(std::unique_ptr<Mesh>& mesh, TransformComponent tr);

	void RenderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc);



	GLuint GetProgram();

protected:

	GLuint program_;

	mat4x4 model_, view_, projection_, mvp_;
	GLuint VAO, VBO, EBO;
};

#endif // PRT