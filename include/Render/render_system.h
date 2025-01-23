
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

	size_t createMesh(ECSManager &ecs, const std::shared_ptr<Mesh>& entity_mesh, Shader& mesh_shader);

	size_t createTerrain(ECSManager& ecs, Shader& shader,int cols ,int rows);

	size_t createSphere(ECSManager& ecs, Shader& shader, int num_heights_, int num_revs_, float size);
	
	//Lights
	size_t createSpotLight(ECSManager& ecs, const Shader& light_shader);

	size_t createPointLight(ECSManager& ecs, const Shader& light_shader);

	size_t createDirectionalLight(ECSManager& ecs, const Shader& light_shader);

private:

	size_t entity_ = 0;
	int number_lights = 0;

};

#endif // RST