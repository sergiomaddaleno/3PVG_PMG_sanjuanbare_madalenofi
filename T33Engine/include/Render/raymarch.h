#ifndef __RAYMARCH__
#define __RAYMARCH__

#include "../Systems/ecs_manager.h"

#include "Core/shader.h"
#include "Core/texture.h"
#include "Geometry/Cube.h"

class RayMarch{

public:
	RayMarch(ECSManager& ecs);
	~RayMarch();

	void initRaymarchShader( std::string vertex_path = "../stages/shaders/raymarch.vs",
							  std::string fragment_path = "../stages/shaders/raymarch.fs");
	void loadNoiseTexture2D(std::string path = "../stages/assets/textures/noise/noise.png");
	void initBoundingBox(float size = 10.0);

	void render(float time);
	void resize(unsigned int width, unsigned int height);

	int window_width, window_height;

	Shader shader_;
	Texture noise_2D_;

	Cube cube;
	float cube_size_;

	glm::vec3 light_color_;
	glm::vec3 absorption_;
	glm::vec3 cloud_direction_;
	float march_size_;
	int max_steps_;
	int max_steps_light_;
	float speed_;
	float amplitude_;
	float frequency_;

private:
	ECSManager& ecs_;

	unsigned int quadVAO, quadVBO;
};

#endif // !__RAYMARCH__
