
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
#include "ecs_manager.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

struct RenderData {

	Camera cam;
	RenderComponent render_comp;
	TransformComponent transf_comp;
	ECSManager ecs;

	int w_width = 800;
	int w_height = 600;
	float fov = 55.0f;
};

class Render {

public:

	Render();

	static std::optional<Render> make();

	void setupShader(GLuint &program, const char* vertex_src, const char* fragment_src);
	
	// --- Normal Components
	void renderGeometry(RenderData& rd);

	void renderMesh(RenderData& rd);

	void renderSkyBox(RenderData& rd);

	// --- For ECS
	void initMeshBuffers(ECSManager &ecs);

	void renderSystem(RenderData& rd);
	// ---

private:

	float skybox_angle;
};

#endif // PRT