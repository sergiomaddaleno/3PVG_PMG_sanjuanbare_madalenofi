
#include "Render/raymarch.h"
#include "../include/Components/components.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "utils.h"

RayMarch::RayMarch(ECSManager& ecs) :
    ecs_(ecs),
    window_width(800), window_height(600)
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

    screen_quad_.init();
    screen_quad_.bind();
}

void RayMarch::render(float time)
{
    shader_.useProgram();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

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
            glm::mat4x4 view = camera_comp->camera.getViewMatrix();
            glm::mat4 projection = camera_comp->camera.getUsePerspectiveMatrix() ? camera_comp->camera.getPerspectiveMatrix() : camera_comp->camera.getOrthograficMatrix();

            shader_.setUniformValue(shader_.getUniformLocation("u_camera_pos"), ValueType::T_FLOAT_3, &camera_position);
            shader_.setUniformValue(shader_.getUniformLocation("view"), ValueType::T_MAT_4x4, &view);
            shader_.setUniformValue(shader_.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection);

            screen_quad_.draw();
        }
    }

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

