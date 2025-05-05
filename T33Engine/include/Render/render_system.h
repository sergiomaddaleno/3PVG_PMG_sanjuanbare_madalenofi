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
#include "Render/ssao.h"

#include "../Systems/ecs_manager.h"
#include "Core/camera.h"
#include "Core/input.h"
#include "Render/skybox.h"
#include "Render/deferred_shading.h"
#include "Render/outlining.h"

#include "Components/components.h"

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

	void initRenderTypes();
	void initLights();
	void initSkybox();

	void initshaders() {
		initShader(shader, "../stages/shaders/forward_shading.vs", "../stages/shaders/forward_shading.fs");
		initShader(texture_shader, "../stages/shaders/basic/texture.vs", "../stages/shaders/basic/texture.fs");

		deferred.initDeferredShaders();
		ssao.initSSAOshaders();
		stencil_outline.initOutlineShader();
	}

	void forwardRender(Input &input, double dt);
	void deferredRender(Input& input, double dt);
	void SSAORender(Input& input, double dt);

	void setupEntities() {
		entity_ = ecs_.create_entity();

		ecs_.add_component_type<IdentifierComponent>();
		ecs_.add_component_type<HierarchyComponent>();
		ecs_.add_component_type<RenderComponent>();
		ecs_.add_component_type<TransformComponent>();
		ecs_.add_component_type<LightComponent>();
		ecs_.add_component_type<PhysicsComponent>();
		ecs_.add_component_type<CameraComponent>();
		ecs_.add_component_type<SkyboxComponent>();
		ecs_.add_component_type<ScriptingComponent>();
	}

	void resize(unsigned int width, unsigned int height);

	//----------------------------------

	size_t addCamera(Camera camera = Camera());

	size_t addSkybox(Skybox skybox = Skybox());

	size_t createCloud(float size = 1.0f, TransformComponent tc = TransformComponent());

	size_t createMesh(std::string file, std::string textureFolder, 
		TransformComponent tc = TransformComponent());

	size_t createTerrain(TransformComponent tc = TransformComponent(),
		const int num_cols = 2, const int num_rows = 2,
		const float height_mult = 1.0f, const float size = 1.0f,
		const bool is_centered = true, const bool use_perlin_noise = true);

	size_t createWater(TransformComponent tc = TransformComponent(), 
		const int num_cols = 2, const int num_rows = 2, 
		const float size = 1.0f, const bool is_centered = true);

	size_t createSphere(TransformComponent tc = TransformComponent(),
		int num_heights_ = 60, int num_revs_ = 60, float size = 1.0f);

	size_t createCube8v(TransformComponent tc = TransformComponent(),
		const float size = 1.0f, bool is_centered = true);

	size_t createSurface(std::vector<float> surface_points,
		TransformComponent tc = TransformComponent(), 
		const int num_heights = 4, const int num_revs = 4);

	size_t createCube24v(TransformComponent tc = TransformComponent(),
		const float size = 1.0f);

	size_t createQuad(TransformComponent tc = TransformComponent(),
		const float size = 1.0f);
	
	size_t createSpotLight(Light li = Light(), bool default_light = true);

	size_t createPointLight(Light li = Light(), bool default_light = true);

	size_t createDirectionalLight(Light li = Light(), bool default_light = true);

	void spawnEntitiesInTerrain(
		int terrain_id, RenderType spawn_type,
		float spawn_prob = 0.002f, float y_spawn_height = -4.0f);

	void spawnMeshInTerrain(
		int terrain_id, std::string file, std::string textureFolder,
		float spawn_prob = 0.002f, float y_spawn_height = -4.0f);
	
	//-----------------------

	glm::mat4 getWorldTransform(int entity);

	Shader shader;
	Shader texture_shader;

	Deferred deferred;
	SSAO ssao;
	Outlining stencil_outline;
	Terrain grid;
	ECSManager& ecs_;

	size_t entity_;

	int window_width;
	int window_height;

	float alpha;
	float alpha_blend;

	WhatToRender what_to_render;

	glm::vec3 sky_colour;
	bool use_fog;
	float fog_density;
	float fog_gradient;

	bool use_skybox;
	bool is_playing;

	EntityCounters entity_counts_;

private:
	bool light_in_scene;

	void renderScene(RenderComponent& rc, Shader& shader);


	void setUniforms(int entity, Shader& shader, Camera& cam);
};

#endif // __RENDER_SYSTEM__

