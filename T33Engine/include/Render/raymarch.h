#ifndef __RAYMARCH__
#define __RAYMARCH__

#include "../Systems/ecs_manager.h"
#include "Core/shader.h"
#include "Core/texture.h"
#include "Geometry/Cube.h"
#include "../deps/nlohmann/json.hpp"

class RayMarch{

public:
	RayMarch(ECSManager& ecs);
	~RayMarch();

	void initRaymarchShader( std::string vertex_path = "../stages/shaders/raymarch.vs",
							  std::string fragment_path = "../stages/shaders/raymarch.fs");
	void loadNoiseTexture2D(std::string path = "../stages/assets/textures/noise/noise.png");
	void loadBlueNoiseTexture2D(std::string path = "../stages/assets/textures/noise/blue-noise-pattern.png");
	void loadNoiseTexture3D(std::string path = "../stages/assets/textures/noise/HDR_LA_0.png");

	void initBoundingBox(float size = 1.0f);

	void render(float time);
	void resize(unsigned int width, unsigned int height);

	int window_width, window_height;

	Shader shader_;
	Texture noise_2D_;
	Texture noise_3D_;
	Texture blue_noise_2D_;

	Cube cube;
	float cube_size_;

	glm::vec3 absorption_;
	glm::vec3 direction_;
	float march_size_;
	int max_steps_;
	int max_steps_light_;
	float speed_;
	float amplitude_;
	float frequency_;
	bool use_blue_noise_;
	float density_;

	nlohmann::json serialize() const;

private:
	ECSManager& ecs_;

};

#endif // !__RAYMARCH__
