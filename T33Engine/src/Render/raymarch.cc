
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

void RayMarch::render(float time)
{
    shader_.useProgram();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float ti = (float)time;
    shader_.setUniformValue(shader_.getUniformLocation("u_time"), ValueType::T_FLOAT_1, &ti);

    glm::vec2 resolution = { window_width, window_height };
    shader_.setUniformValue(shader_.getUniformLocation("u_resolution"), ValueType::T_FLOAT_2, &resolution);

    noise_2D_.active(GL_TEXTURE0);
    noise_2D_.bind2D();
    int tex_id = GL_TEXTURE0;
    shader_.setUniformValue(shader_.getUniformLocation("u_noise_2D"), ValueType::T_INT_1, &tex_id);

    /*noise_3D_.active(GL_TEXTURE1);
    noise_3D_.bind3D();
    tex_id = GL_TEXTURE1;
    shader_.setUniformValue(shader_.getUniformLocation("u_noise_3D"), ValueType::T_INT_1, &tex_id);*/

    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();

    for (; ci != ciend; ++ci) {
        if (ci->has_value()) {
            auto& camera_comp = *ci;

            glm::vec3 camera_position = camera_comp->camera.getPosition();
            glm::mat4 view = camera_comp->camera.getViewMatrix();
            glm::mat4 projection = camera_comp->camera.getUsePerspectiveMatrix() ? camera_comp->camera.getPerspectiveMatrix() : camera_comp->camera.getOrthograficMatrix();

            shader_.setUniformValue(shader_.getUniformLocation("u_camera_pos"), ValueType::T_FLOAT_3, &camera_position);
            shader_.setUniformValue(shader_.getUniformLocation("view"), ValueType::T_MAT_4x4, &view);
            shader_.setUniformValue(shader_.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection);

            screen_quad_.drawVec2();
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

