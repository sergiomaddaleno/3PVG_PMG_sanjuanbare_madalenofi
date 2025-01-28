
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


struct LightInfo;

class Mesh;
class Shader;
class Terrain;

class RenderSystem {

public:

	RenderSystem();
	~RenderSystem();


	void initEntities(ECSManager &ecs);

	void renderEntities(ECSManager& ecs, Camera cam = Camera());

	void renderMesh(ECSManager& ecs, Camera cam = Camera());

	//---------------------------------------------------------------
	
	//call one time to create entites
	void setupEntities(ECSManager& ecs);
	//--

	size_t createMesh(ECSManager &ecs, const std::shared_ptr<Mesh>& entity_mesh, Shader& mesh_shader, std::string& file, std::string& textureFolder, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f));

	size_t createTerrain(ECSManager& ecs, Shader& shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f),  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f),
		const int num_cols = 2, const int num_rows = 2,
		const float height_mult = 1.0f, const float size = 1.0f,
		const bool is_centered = true, const bool use_perlin_noise = true);

	size_t createSphere(ECSManager& ecs, Shader& shader, glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), int num_heights_ = 60, int num_revs_ = 60, float size = 1.0f);

	size_t createCube8v(ECSManager& ecs, Shader& shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), const float size = 1.0f);

	size_t createCube24v(ECSManager& ecs, Shader& shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), const float size = 1.0f);

	size_t createQuad(ECSManager& ecs, Shader& shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), const float size = 1.0f);
	
	//Lights
	size_t createSpotLight(ECSManager& ecs, const Shader& light_shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(255.0f, 255.0f, 255.0f));

	size_t createPointLight(ECSManager& ecs, Shader& light_shader, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 rotate = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 color = glm::vec3(255.0f, 255.0f, 255.0f));

	size_t createDirectionalLight(ECSManager& ecs, const Shader& light_shader);
	
	int number_lights = 0;

private:

	size_t entity_ = 0;
};

#endif // RST