
#ifndef RST
#define RST

#include <optional>
#include <string>
#include <vector>
#include <memory>

#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Systems/ecs_manager.h"
#include "Render/camera.h"


#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"

class Mesh;
class Shader;
class Terrain;

class RenderSystem {

public:

	RenderSystem();
	~RenderSystem();

	void initEntities(ECSManager &ecs);

	void setEntitiesUniformsToCalculateShadow(TransformComponent tc, Shader shader, Camera cam);

	void renderShadowMap(ECSManager& ecs, Shader shader, Camera cam = Camera());

	void renderEntities(ECSManager& ecs, Shader shader, Camera cam = Camera());

	//--

	void renderMesh(ECSManager& ecs, Camera cam = Camera());

	void setEntitiesUniforms(TransformComponent tc, Shader shader, Camera cam);
	
	//---------------------------------------------------------------
	
	//call one time to create entites
	void setupEntities(ECSManager& ecs);

	void GenerateMaps(ECSManager& ecs);

	//--

	size_t createMesh(ECSManager &ecs, Shader& mesh_shader, std::string file, std::string textureFolder, 
		TransformComponent tc = TransformComponent());

	size_t createTerrain(ECSManager& ecs, Shader& shader, TransformComponent tc = TransformComponent(),
		const int num_cols = 2, const int num_rows = 2,
		const float height_mult = 1.0f, const float size = 1.0f,
		const bool is_centered = true, const bool use_perlin_noise = true);

	size_t createSphere(ECSManager& ecs, Shader& shader, 
		TransformComponent tc = TransformComponent(),
		int num_heights_ = 60, int num_revs_ = 60, float size = 1.0f);

	size_t createCube8v(ECSManager& ecs, Shader& shader, 
		TransformComponent tc = TransformComponent(),
		const float size = 1.0f);

	size_t createCube24v(ECSManager& ecs, Shader& shader, 
		TransformComponent tc = TransformComponent(),
		const float size = 1.0f);

	size_t createQuad(ECSManager& ecs, Shader& shader, 
		TransformComponent tc = TransformComponent(),
		const float size = 1.0f);
	
	//Lights
	size_t createSpotLight(ECSManager& ecs, const Shader& light_shader, 
		LightInfo li = LightInfo(), bool default_light = true);

	size_t createPointLight(ECSManager& ecs, Shader& light_shader, 
		LightInfo li = LightInfo(), bool default_light = true);

	size_t createDirectionalLight(ECSManager& ecs, const Shader& light_shader, 
		LightInfo li = LightInfo(), bool default_light = true);
	
	int number_lights = 0;

private:

	size_t entity_ = 0;
};

#endif // RST