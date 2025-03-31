#pragma once
#include "../clouds/ScreenSpaceShader.h"
#include "../clouds/buffers.h"
#include "../clouds/texture.h"
#include "../include/Core/shader.h"


#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <../include/Core/camera.h>
#include<../include/Managers/engine.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../include/Components/camera_component.h>
#include <algorithm>
#include "drawableObject.h"
#include "CloudsModel.h"

//VolumetricClouds handles the FrameBufferObjects (and textures) where the clouds will be rendered, and it's responsible to set up the uniforms and calling the draw command.
class VolumetricClouds 
{
public:
	VolumetricClouds(int SW, int SH, CloudsModel * model);
	 void draw(Engine& s);
	~VolumetricClouds();

	enum cloudsTextureNames {fragColor, bloom, alphaness, cloudDistance};

	void generateWeatherMap();

	unsigned int getCloudsTexture() { 
		return (model->postProcess ? cloudsPostProcessingFBO->getColorAttachmentTex(0) : getCloudsRawTexture());
	}

	unsigned int getCloudsTexture(int i) {
		return cloudsFBO->getColorAttachmentTex(i);
	}

	unsigned int getCloudsRawTexture(){
		return cloudsFBO->getColorAttachmentTex(0);
	}



private:
	int SCR_WIDTH, SCR_HEIGHT;

	TextureSet * cloudsFBO;
	FrameBufferObject *cloudsPostProcessingFBO;

	CloudsModel * model;
};

