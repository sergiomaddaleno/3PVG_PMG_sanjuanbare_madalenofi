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

#include "Geometry/screen_quad.h"
#include "utils.h"

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

    Deferred(ECSManager& ecs);
    ~Deferred();

    void initDeferredShaders() {
        initShader(gbuffer, "../stages/shaders/deferred/g_buffer.vs", "../stages/shaders/deferred/g_buffer.fs");
        initShader(deferred_shading, "../stages/shaders/deferred/deferred_shading.vs", "../stages/shaders/deferred/deferred_shading.fs");
    }
    void init();
    void render(Camera cam);
    void resize(unsigned int width, unsigned int height);

    glm::mat4 getWorldTransform(int entity);

    Shader gbuffer, deferred_shading;

    int deferred_option;

private:
    unsigned int gBuffer, rboDepth;
    unsigned int gPosition, gNormal, gAlbedoSpec;

    unsigned int width_;
    unsigned int height_;

    ScreenQuad screen_quad_;
    ECSManager& ecs_;
};

#endif // !__DEFERRED__