#ifndef __RAYMARCH__
#define __RAYMARCH__

#include "../Systems/ecs_manager.h"
#include "../Render/render_system.h"

#include "Core/shader.h"
#include "Core/texture.h"
#include "Core/framebuffer.h"
#include "Geometry/quad.h"
#include "Geometry/cube.h"

class RayMarch{

public:
	RayMarch(ECSManager& ecs);
	~RayMarch();

	void initRaymarchShader(
		std::string vertex_path = "../stages/shaders/raymarch.vs",
		std::string fragment_path = "../stages/shaders/raymarch.fs"
	);

	void init() {
		initRaymarchShader();

		noise_2D_.loadTexture("../stages/assets/textures/noise/noise.png");
		noise_3D_.loadTexture3D("../stages/assets/textures/noise/HDR_L.raw");

		screen_quad_.init();
		screen_quad_.bind();
	}

	void render(float time);

	void resize(unsigned int width, unsigned int height);

	int window_width, window_height;

	Shader shader_;

	Texture noise_2D_;
	Texture noise_3D_;

	Quad screen_quad_;
	FrameBuffer framebuffer_;
private:

	ECSManager& ecs_;

	unsigned int quadVAO, quadVBO;

	std::string vertex_path_;
	std::string fragment_path_;
};

#endif // !__RAYMARCH__
