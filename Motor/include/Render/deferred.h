
#ifndef __DEFERRED__
#define __DEFERRED__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>
#include "Core/shader.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ECSManager;
class Shader;
class Camera;
struct LightComponent;

class Deferred {

public:

	Deferred();
	~Deferred();

	void init();

	void render(ECSManager& ecs, Camera cam);

	Shader gbuffer, deferred_shading;

private:
	unsigned int gBuffer, rboDepth;
	unsigned int gPosition, gNormal, gAlbedoSpec;

	unsigned int deferred_width_;
	unsigned int deferred_height_;
};

#endif // __DEFERRED__