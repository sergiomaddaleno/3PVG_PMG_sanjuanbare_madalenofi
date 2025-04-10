/**
 * @file deferred_shading.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

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

/**
 * @class Deferred
 * @brief Implements deferred rendering technique for 3D scenes.
 */
class Deferred {

public:

    Deferred();
    ~Deferred();

    void init();

    void render(ECSManager& ecs, Camera cam);
    void renderQuad();

    void resize(unsigned int width, unsigned int height);

    Shader gbuffer, deferred_shading;

    int deferred_option;

private:
    unsigned int gBuffer, rboDepth;
    unsigned int gPosition, gNormal, gAlbedoSpec, shadowMap;

    unsigned int quadVAO, quadVBO;

    glm::mat4 u_light_space;

    unsigned int width_;
    unsigned int height_;
};

#endif // !__DEFERRED__