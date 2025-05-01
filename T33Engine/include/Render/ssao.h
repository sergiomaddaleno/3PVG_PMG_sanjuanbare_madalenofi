/**
 * @file ssao.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SSAO__
#define __SSAO__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/glm/glm.hpp>

#include <optional>
#include <string>
#include <cstdlib>  
#include <ctime>
#include <random>

#include "Core/shader.h"
#include "Geometry/quad.h"
#include "Geometry/screen_quad.h"

class ECSManager;
class Camera;

/**
 * @class SSAO
 * @brief Implements SSAO rendering technique for 3D scenes.
 */
class SSAO {

public:

	SSAO(ECSManager& ecs);
	~SSAO();

	void init();
	void render(Camera cam);
	void resize(unsigned int width, unsigned int height);

	glm::mat4 getWorldTransform(int entity);

	Shader shader, geometry, light_pass, blur;

	int kernel_size;
	float radius;
	float bias;
	int ssao_option;

private:
	unsigned int ssaoFBO;
	unsigned int ssaoBlurFBO;

	unsigned int ssaoColorBuffer;
	unsigned int ssaoColorBufferBlur;

	std::uniform_real_distribution<GLfloat> randomFloats; 
	std::default_random_engine generator;

	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;

	unsigned int noiseTexture;

	unsigned int gBuffer, rboDepth;
	unsigned int gPosition, gNormal, gAlbedoSpec;

	unsigned int width_;
	unsigned int height_;

	ScreenQuad quad;
	ECSManager& ecs_;
};

#endif // !__SSAO__