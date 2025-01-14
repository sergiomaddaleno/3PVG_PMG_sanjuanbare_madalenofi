
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
#include "..\include\Components\light_component.h"


struct RenderData;

class Render {

public:

	Render();

	// --- Normal Components
	void renderGeometry(RenderData& rd);

	void renderMesh(RenderData& rd);

	// --- For ECS
	void initMeshBuffers(ECSManager &ecs);

	void renderSystem(ECSManager& ecs, RenderData& rd);

	void renderLightSystem(ECSManager& ecs);
	// ---

};

#endif // PRT