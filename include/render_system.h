
#ifndef RST
#define RST

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ecs_manager.h"

struct RenderData;

class RenderSystem {

public:

	RenderSystem();
	~RenderSystem();

	// Mesh
	void initMesh(ECSManager &ecs);

	void renderMesh(ECSManager& ecs, RenderData& rd);

	// ---------------

	// Light
	void initLight(ECSManager& ecs);

	void renderLight(ECSManager& ecs, RenderData& rd);

};

#endif // RST