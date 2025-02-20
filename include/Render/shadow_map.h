
#ifndef __SHADOW_MAP__
#define __SHADOW_MAP__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class ECSManager;
class Shader;
class Camera;
struct LightComponent;

class ShadowMap {

public:

	ShadowMap();
	~ShadowMap();

	void init();
	void initCubeMap();

	void bind();
	void render(ECSManager& ecs, Shader shader, LightComponent lc);

	float near_plane, far_plane;
	unsigned int shadow_width_;
	unsigned int shadow_height_;

	float directional_size_;

private:
	unsigned int depthMapFBO;
	unsigned int depthMap;

	unsigned int depthCubemap;
};

#endif // __SHADOW_MAP__