#ifndef __RAYMARCH__
#define __RAYMARCH__

#include "../Systems/ecs_manager.h"
#include "../Render/render_system.h"

#include "Core/shader.h"
#include "Geometry/sphere.h"

class RayMarch{

public:

	RayMarch(ECSManager& ecs);
	~RayMarch();

	void initRaymarchShader(
		std::string vertex_path = "../stages/shaders/raymarch.vs",
		std::string fragment_path = "../stages/shaders/raymarch.fs"
	);

	void init();
	void render(double time);

	void resize(unsigned int width, unsigned int height);

	int window_width, window_height;
	double DPR;

	Shader shader_;

private:

	ECSManager& ecs_;

	unsigned int quadVAO, quadVBO;

	std::string vertex_path_;
	std::string fragment_path_;
};

#endif // !__RAYMARCH__
