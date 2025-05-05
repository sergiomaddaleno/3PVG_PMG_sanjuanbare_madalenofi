/**
 * @file ssao.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __OUTLINING__
#define __OUTLINING__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/glm/glm.hpp>

#include <optional>
#include <string>
#include <cstdlib>  
#include <ctime>
#include <random>

#include "Core/shader.h"
#include "../Systems/ecs_manager.h"

class Camera;
struct RenderComponent;

/**
 * @class Outlining
 * @brief Implements Stencil Buffer & Outlining for model entities.
 */
class Outlining {

public:

	Outlining(ECSManager& ecs);
	~Outlining();

	void initOutlineShader(
		std::string vertex_path = "../stages/shaders/outlining/outline.vs",
		std::string fragment_path = "../stages/shaders/outlining/outline.fs");
	void init();
	void render(Shader& scene_shader, glm::mat4 camera_matrix);

	glm::mat4 getWorldTransform(int entity);

	Shader shader;
	float outline_ = 1.08f;

private:
	void renderScene(RenderComponent& rc, Shader& shader);

	ECSManager& ecs_;
};

#endif // !__SSAO__