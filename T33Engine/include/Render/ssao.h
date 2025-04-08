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

	SSAO();
	~SSAO();

	void init();
	void render(ECSManager& ecs, Camera cam);
	void renderQuad();
	
	void resize(unsigned int width, unsigned int height);

	Shader ssao_shader, ssao_geometry;

private:
	unsigned int ssaoFBO;
	unsigned int ssaoColorBuffer;

	std::uniform_real_distribution<GLfloat> randomFloats; 
	std::default_random_engine generator;

	std::vector<glm::vec3> ssaoKernel;
	std::vector<glm::vec3> ssaoNoise;

	unsigned int noiseTexture;

	unsigned int gBuffer, rboDepth;
	unsigned int gPosition, gNormal, gAlbedoSpec;

	unsigned int quadVAO, quadVBO;

	ScreenQuad quad;

	unsigned int width_;
	unsigned int height_;
};

#endif // !__SSAO__