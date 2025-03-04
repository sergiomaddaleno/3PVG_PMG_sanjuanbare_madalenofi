
#ifndef __RENDER_SYSTEM__
#define __RENDER_SYSTEM__

#include <optional>
#include <string>
#include <vector>
#include <memory>

#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Systems/ecs_manager.h"
#include "Render/camera.h"
#include "Render/deferred_shading.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"

class Mesh;
class Shader;
class Terrain;
class Shader;

class RenderSystem {

public:

	RenderSystem();
	~RenderSystem();

	void init(ECSManager &ecs);

	void setUniforms(TransformComponent tc, Shader shader, LightComponent light, Camera cam);

	void forwardRender(ECSManager& ecs, Camera cam = Camera());

	void deferredRender(ECSManager& ecs, Camera cam = Camera());

	//call one time to create entites
	void setup(ECSManager& ecs);

	//----------------------------------

	size_t createMesh(ECSManager &ecs, std::string file, std::string textureFolder, 
		TransformComponent tc = TransformComponent());

	size_t createTerrain(ECSManager& ecs, TransformComponent tc = TransformComponent(),
		const int num_cols = 2, const int num_rows = 2,
		const float height_mult = 1.0f, const float size = 1.0f,
		const bool is_centered = true, const bool use_perlin_noise = true);

	size_t createSphere(ECSManager& ecs, TransformComponent tc = TransformComponent(),
		int num_heights_ = 60, int num_revs_ = 60, float size = 1.0f);

	size_t createCube8v(ECSManager& ecs, TransformComponent tc = TransformComponent(),
		const float size = 1.0f, bool is_centered = true);

	size_t createCube24v(ECSManager& ecs, TransformComponent tc = TransformComponent(),
		const float size = 1.0f);

	size_t createQuad(ECSManager& ecs, TransformComponent tc = TransformComponent(),
		const float size = 1.0f);
	
	size_t createSpotLight(ECSManager& ecs, 
		Light li = Light(), bool default_light = true);

	size_t createPointLight(ECSManager& ecs, 
		Light li = Light(), bool default_light = true);

	size_t createDirectionalLight(ECSManager& ecs, 
		Light li = Light(), bool default_light = true);
	
	//-----------------------

	Shader shader;
	Deferred deferred;
	size_t entity_;

	int entity_mesh;
	int entity_terrain;
	int entity_sphere;
	int entity_cube8v;
	int entity_cube24v;
	int entity_quad;
	int entity_spotlight;
	int entity_pointlight;
	int entity_dirlight;

	//Dont use
	void compileShaderForBuild();
};

#endif // __RENDER_SYSTEM__