/**
 * @file render_system.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

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
#include "Core/camera.h"
#include "Render/deferred_shading.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"

class Mesh;
class Shader;
class Terrain;
class Shader;

/**
 * @class RenderSystem
 * @brief Manages rendering of 3D scenes, including entity creation and rendering techniques.
 */
class RenderSystem {

public:

	RenderSystem(ECSManager& ecs);
	~RenderSystem();

	void init();

	void forwardRender(Camera cam = Camera());

	void deferredRender(Camera cam = Camera());

	//call one time to create entites
	void setup();

	//----------------------------------

	size_t createMesh(std::string file, std::string textureFolder, 
		TransformComponent tc = TransformComponent());

	size_t createTerrain(TransformComponent tc = TransformComponent(),
		const int num_cols = 2, const int num_rows = 2,
		const float height_mult = 1.0f, const float size = 1.0f,
		const bool is_centered = true, const bool use_perlin_noise = true);

	size_t createSphere(TransformComponent tc = TransformComponent(),
		int num_heights_ = 60, int num_revs_ = 60, float size = 1.0f);

	size_t createCube8v(TransformComponent tc = TransformComponent(),
		const float size = 1.0f, bool is_centered = true);

	size_t createCube24v(TransformComponent tc = TransformComponent(),
		const float size = 1.0f);

	size_t createQuad(TransformComponent tc = TransformComponent(),
		const float size = 1.0f);
	
	size_t createSpotLight(Light li = Light(), bool default_light = true);

	size_t createPointLight(Light li = Light(), bool default_light = true);

	size_t createDirectionalLight(Light li = Light(), bool default_light = true);
	
	//-----------------------

	Shader shader;
	Deferred deferred;
	ECSManager& ecs_;
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

private:

	void setUniforms(TransformComponent tc, Shader shader, LightComponent light, Camera cam);

	void compileShaderForBuild();
};

#endif // __RENDER_SYSTEM__