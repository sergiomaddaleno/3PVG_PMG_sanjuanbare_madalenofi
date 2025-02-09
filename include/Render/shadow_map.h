
#ifndef __SHADOW_MAP__
#define __SHADOW_MAP__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include "Components\light_component.h"

#include <../nlohmann/json.hpp>

class ECSManager;
class Shader;
class Camera;
struct LightComponent;

class ShadowMap {

public:

	ShadowMap();
	~ShadowMap();

	void initOneShadow();
	void initMultipleShadow();

	void addNewShadow(int light_count);
	void bind();

	void render(ECSManager& ecs, Shader shader, Camera cam, LightComponent lc, int light_count);

	unsigned int depthMapFBO = 0;
	unsigned int depthMap = 0;

	float near_plane = 1.0f, far_plane = 100.5f;
	int multiple_shadows_ = 2;
	unsigned int shadow_width_ = 2048;
	unsigned int shadow_height_ = 2048;

};

#endif // __SHADOW_MAP__