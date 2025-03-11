/**
 * @file shadow_map.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SHADOW_MAP__
#define __SHADOW_MAP__

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

/**
 * @class ShadowMap
 * @brief Implements shadow mapping techniques for 3D rendering.
 */
class ShadowMap {

public:

	ShadowMap();
	~ShadowMap();

	void init2D();
	void initCubeMap();

	void active(unsigned int num = 0);
	void bind2D();
	void bindCubeMap();

	void render(ECSManager& ecs, Shader shader, LightComponent lc);

	float near_plane, far_plane;
	unsigned int shadow_width_;
	unsigned int shadow_height_;
	unsigned int countfaces = 0;
	float directional_size_;

	Shader simple_depth;
private:
	void renderScene(ECSManager& ecs, Shader shader, LightComponent lc);
	void renderCubeMap(ECSManager& ecs, Shader shader, LightComponent lc);

	unsigned int depthMapFBO;
	unsigned int depthMap;
	unsigned int depthCubemap;

	glm::mat4 lightProjection;
	glm::mat4 lightView;
	glm::mat4 lightSpaceMatrix;
};

#endif // !__SHADOW_MAP__