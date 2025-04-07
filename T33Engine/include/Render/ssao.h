
/**
 * @file ssao.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SSAO__
#define __SSAO__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>
#include "Core/shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdlib>  
#include <ctime>

#include "Geometry/quad.h"
#include "Geometry/screen_quad.h"
#include <random>

class ECSManager;
class Shader;
class Camera;
struct LightComponent;


/**
 * @class Deferred
 * @brief Implements deferred rendering technique for 3D scenes.
 */
class SSAO {

public:

	SSAO();
	~SSAO();

	float ourLerp(float a, float b, float f);

	float randFloat();

	void init();

	void render(ECSManager& ecs, Camera cam);


	void renderQuad();
	
	
	Shader ssao_shader,ssao_geometry;

	void resize(unsigned int width, unsigned int height);
	unsigned int quadVAO, quadVBO;

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

	ScreenQuad quad;

	unsigned int width_;
	unsigned int height_;
};

#endif // !__SSAO__