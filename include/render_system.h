
#ifndef RST
#define RST

#include <optional>
#include <string>
#include <vector>
#include <memory>

#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ecs_manager.h"
#include "mesh.h"
#include "shader.h"

struct RenderData;
struct LightInfo;

class RenderSystem {

public:

	RenderSystem();
	~RenderSystem();


	void initEntities(ECSManager &ecs);

	void renderEntities(ECSManager &ecs, RenderData& rd);

	void renderMesh(ECSManager& ecs, RenderData& rd);

	//---------------------------------------------------------------
	
	//call one time to create entites
	void setupEntities(ECSManager& ecs);
	//--

	size_t createMesh(ECSManager &ecs, const std::shared_ptr<Mesh>& entity_mesh, const Shader& mesh_shader);
	
	//Lights
	size_t createSpotLight(ECSManager &ecs, const std::shared_ptr<Mesh> &light_bulb_mesh, const Shader& mesh_shader, const Shader& light_shader);

	size_t createPointLight(ECSManager& ecs, const std::shared_ptr<Mesh>& light_bulb_mesh, const Shader& mesh_shader, const Shader& light_shader);

	size_t createDirectionalLight(ECSManager& ecs, const std::shared_ptr<Mesh>& light_bulb_mesh, const Shader& mesh_shader, const Shader& light_shader);

private:

	size_t entity_ = 0;
	int number_lights = 0;
};

#endif // RST