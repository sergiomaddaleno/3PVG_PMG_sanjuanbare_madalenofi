
#ifndef PRR
#define PRR

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

	void setupShader(ShaderBuffers &shaders);
	// ---

	void initMeshBuffers(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc);


	void renderGeometry(RenderComponent& rc, TransformComponent& tr);

	void renderMesh(RenderComponent& rc, TransformComponent& tc);

	void renderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc);



	GLuint getProgram();

protected:

	GLuint program_;

	glm::mat4x4 model_, view_, projection_, mvp_;
};

#endif // PRT