#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../clouds/Shader2.h"
#include <../deps/glm/glm.hpp>
#include "../clouds/ScreenSpaceShader.h"


#define INT_CEIL(n,d) (int)ceil((float)n/d)

//CloudsModel is responsible to collect the attributes and shaders that will be needed to render the volumetric clouds. Also, it creates the noises which models the clouds.
class CloudsModel
{
public:
	friend class VolumetricClouds;

	CloudsModel();
	~CloudsModel();
	
	// fake implementation, it's needed to let this class being a drawableObject to subscribe to GUI class. must be fixed
	virtual void draw() {};

	virtual void update();


private:
	Shader2 * volumetricCloudsShader, *weatherShader;
	ScreenSpaceShader * postProcessingShader;

	float coverage, cloudSpeed, crispiness, curliness, density, absorption;
	float earthRadius, sphereInnerRadius, sphereOuterRadius;
	float perlinFrequency;
	bool enableGodRays;
	bool enablePowder;
	bool postProcess;

	glm::vec3 cloudColorTop, cloudColorBottom;

	glm::vec3 seed, oldSeed;
	unsigned int perlinTex, worley32, weatherTex;


	void generateWeatherMap();
	void generateModelTextures();
	void initVariables();
	void initShaders();


};

