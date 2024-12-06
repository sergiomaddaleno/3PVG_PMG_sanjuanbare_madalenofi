
#ifndef PRR
#define PRR

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "triangle.h"
#include "mesh.h"
#include "camera.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

struct RenderData {

	Camera cam;
	RenderComponent rc;
	TransformComponent tc;
	std::vector<std::optional<RenderComponent>> rc_ecs;
	std::vector<std::optional<TransformComponent>> tc_ecs;

	int window_width;
	int window_height;
};

class Render {

public:

	Render();

	static std::optional<Render> make();

	void setupShader(GLuint &program, const char* vertex_src, const char* fragment_src);
	
	// ---

	void renderGeometry(RenderComponent& rc, TransformComponent& tr);

	void renderMesh(RenderComponent& rc, TransformComponent& tc);

	// ---
	void initMeshBuffers(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc);

	void renderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc, Camera& cam, float width, float height);
	// ---

	void renderSkyBox(RenderComponent& rc, Camera& cam, float width, float height);


private:
	glm::mat4x4 model_, projection_, mvp_;

	float skybox_angle = -90.0f;

};

#endif // PRT