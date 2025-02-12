
#ifndef __SHADOW_MAP__
#define __SHADOW_MAP__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ECSManager;
class Shader;
struct LightComponent;

class ShadowMap {

public:

	ShadowMap();
	~ShadowMap();

	void init();
	void bind();
	void render(ECSManager& ecs, Shader shader, LightComponent lc);

	unsigned int depthMapFBO = 0;
	unsigned int depthMap = 0;

	float near_plane = 1.0f, far_plane = 200.5f;
	unsigned int shadow_width_ = 2048;
	unsigned int shadow_height_ = 2048;

};

#endif // __SHADOW_MAP__