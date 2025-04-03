
#include "Render/raymarch.h"
#include "../include/Components/components.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "utils.h"

static float screen_quad[] = {
    -1.0f, -1.0f,   0.0f, 0.0f,
    1.0f, -1.0f,    1.0f, 0.0f,
    1.0f,  1.0f,    1.0f, 1.0f,

    -1.0f, -1.0f,    0.0f, 0.0f,
     1.0f,  1.0f,    1.0f, 1.0f,
    -1.0f,  1.0f,    0.0f, 1.0f
};

RayMarch::RayMarch(ECSManager& ecs) :
    ecs_(ecs),
    window_width(800), window_height(600),
    DPR(0.75)
{
}

RayMarch::~RayMarch()
{
}

void RayMarch::initRaymarchShader(std::string vertex_path, std::string fragment_path)
{
    initShader(shader_, vertex_path, fragment_path);

    vertex_path_ = vertex_path;
    fragment_path_ = fragment_path;
}

void RayMarch::init()
{
    initRaymarchShader();

    noise_.loadTexture("../stages/assets/textures/noise/noise.png");

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), screen_quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RayMarch::render(float time)
{
    shader_.useProgram();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader_.setUniformValue(shader_.getUniformLocation("u_time"), ValueType::T_FLOAT_1, &time);

    glm::vec2 resolution = { window_width, window_height };
    shader_.setUniformValue(shader_.getUniformLocation("u_resolution"), ValueType::T_FLOAT_2, &resolution);

    noise_.active(TextureID::TI_Albedo);
    noise_.bind2D();

    int tex_id = TextureID::TI_Albedo;
    shader_.setUniformValue(shader_.getUniformLocation("u_noise"), ValueType::T_INT_1, &tex_id);

    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();

    for (; ci != ciend; ++ci) {
        if (ci->has_value()) {
            auto& camera_comp = *ci;

            glm::vec3 camera_position = camera_comp->camera.getPosition();
            shader_.setUniformValue(shader_.getUniformLocation("u_ray_origin"), ValueType::T_FLOAT_3, &camera_position);

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }

    glDisable(GL_BLEND);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RayMarch::resize(unsigned int width, unsigned int height)
{
    if (window_width != width || window_height != height) {

        window_width = width;
        window_height = height;
    }
}

