#include "CloudsModel.h"

#include "../clouds/texture.h"




CloudsModel::CloudsModel() 
{
	initVariables();
	initShaders();
	generateModelTextures();
}

void CloudsModel::initShaders()
{
	volumetricCloudsShader = new Shader2("volumetricCloudsShader", "shaders/volumetric_clouds.comp");
	postProcessingShader = new ScreenSpaceShader("shaders/clouds_post.frag");
	//compute shaders
	weatherShader = new Shader2("weatherMap");
	weatherShader->attachShader("shaders/weather.comp");
	weatherShader->linkPrograms();
}

void CloudsModel::generateModelTextures()
{
	/////////////////// TEXTURE GENERATION //////////////////
	if (!perlinTex) {
		//compute shaders
		Shader2 comp("perlinWorley");
		comp.attachShader("shaders/perlinworley.comp");
		comp.linkPrograms();

		//make texture
		this->perlinTex = generateTexture3D(128, 128, 128);
		//compute
		comp.use();
		comp.setVec3("u_resolution", glm::vec3(128, 128, 128));
		std::cout << "computing perlinworley!" << std::endl;
		glActiveTexture(GL_TEXTURE0);
		comp.setInt("outVolTex", 0);
		glBindTexture(GL_TEXTURE_3D, this->perlinTex);
	  	glBindImageTexture(0, this->perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
		std::cout << "computed!!" << std::endl;
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glGenerateMipmap(GL_TEXTURE_3D);
	}

	if (!worley32) {
		//compute shaders
		Shader2 worley_git("worleyComp");
		worley_git.attachShader("shaders/worley.comp");
		worley_git.linkPrograms();

		//make texture
		this->worley32 = generateTexture3D(32, 32, 32);

		//compute
		worley_git.use();
		//worley_git.setVec3("u_resolution", glm::vec3(32, 32, 32));
		glActiveTexture(GL_TEXTURE0);
	  	glBindTexture(GL_TEXTURE_3D, this->worley32);
	  	glBindImageTexture(0, this->worley32, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		std::cout << "computing worley 32!" << std::endl;
		glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
		std::cout << "computed!!" << std::endl;
		glGenerateMipmap(GL_TEXTURE_3D);
	}


	////////////////////////

	if (!weatherTex) {
		//make texture
		this->weatherTex = generateTexture2D(1024, 1024);

		//compute
		generateWeatherMap();

	
	}
}

CloudsModel::~CloudsModel()
{
	delete volumetricCloudsShader;
	delete postProcessingShader;
	delete weatherShader;
}

void CloudsModel::update()
{
	/*
	seed = scene->seed;
	if (seed != oldSeed) {
		generateWeatherMap();
		oldSeed = seed;
	}
	*/
}

void CloudsModel::generateWeatherMap() {
	/*
	bindTexture2D(weatherTex, 0);
	weatherShader->use();
	weatherShader->setVec3("seed", scene->seed);
	weatherShader->setFloat("perlinFrequency", perlinFrequency);
	std::cout << "computing weather!" << std::endl;
	glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
	std::cout << "weather computed!!" << std::endl;

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); */
}

void CloudsModel::initVariables()
{
	cloudSpeed = 450.0;
	coverage = 0.45;
	crispiness = 40.;
	curliness = .1;
	density = 0.02;
	absorption = 0.35;

	earthRadius = 600000.0;
	sphereInnerRadius = 5000.0;
	sphereOuterRadius = 17000.0;

	perlinFrequency = 0.8;

	enableGodRays = false;
	enablePowder = false;
	postProcess = true;

	seed = glm::vec3(0.0, 0.0, 0.0);
	oldSeed = glm::vec3(0.0, 0.0, 0.0);

	cloudColorTop = (glm::vec3(169., 149., 149.)*(1.5f / 255.f));
	cloudColorBottom = (glm::vec3(65., 70., 80.)*(1.5f / 255.f));

	weatherTex = 0;
	perlinTex = 0;
	worley32 = 0;
}
