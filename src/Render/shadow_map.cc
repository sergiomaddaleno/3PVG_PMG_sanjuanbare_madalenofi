
#include "Render/shadow_map.h"
#include "Render/camera.h"
#include "Core/framebuffer.h"

#include "Systems/ecs_manager.h"
#include "Core/shader.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"
#include "global_variables.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <memory> 
#include <iostream>



ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
}

void ShadowMap::initOneShadow()
{
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width_, shadow_height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void ShadowMap::initMultipleShadow(unsigned int fbo)
{
    depthMapFBO = fbo;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, shadow_width_, shadow_height_, multiple_shadows_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void ShadowMap::addNewShadow(int light_count) {
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0, light_count);
}

void ShadowMap::bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap);
}

void ShadowMap::render(ECSManager& ecs, Shader shader, Camera cam, LightComponent lc, int light_count) {

    float spotLightFOV = glm::radians(lc.light.light_info_.spot_angle_);
    glm::mat4 lightProjection = glm::perspective(spotLightFOV, (float)g_window_width_ / (float)g_window_height_, near_plane, far_plane);

    glm::mat4 lightView = glm::lookAt(lc.light.light_info_.pos_, lc.light.light_info_.pos_ + lc.light.light_info_.dir_, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    glEnable(GL_DEPTH_TEST);
    shader.useProgram();
    int loc = shader.getUniformLocation(("u_light_space[" + std::to_string(light_count) + "]").c_str());
    shader.setUniformValue(loc, ValueType::T_MAT_4x4, glm::value_ptr(lightSpaceMatrix));

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(lightView));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(lightProjection));

    /*loc = shader.getUniformLocation(("u_light_view[" + std::to_string(light_count) + "]").c_str());
    shader.setUniformValue(loc, ValueType::T_MAT_4x4, glm::value_ptr(lightView));

    loc = shader.getUniformLocation(("u_light_projection[" + std::to_string(light_count) + "]").c_str());
    shader.setUniformValue(loc, ValueType::T_MAT_4x4, glm::value_ptr(lightProjection));*/

    //bind();
    addNewShadow(light_count);
    //int texture_unit = 0;
    //shader.setUniformValue(shader.getUniformLocation("u_shadow_map"), ValueType::T_INT_1, &texture_unit);

    glViewport(0, 0, shadow_width_, shadow_height_);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform_comp->position);
            model = model * glm::mat4_cast(transform_comp->rotation);
            model = glm::scale(model, transform_comp->scale);
            shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
            case RenderType::RT_SpotLight:
            case RenderType::RT_PointLight:
                for (auto& mesh : render_comp->mesh_->meshes) {
                    render_comp->mesh_->renderElements(false);
                }
                break;
            case RenderType::RT_Terrain:
                render_comp->terrain_->render(false);
                break;
            case RenderType::RT_Sphere:
                render_comp->sphere_->render();
                break;
            case RenderType::RT_Cube:
                render_comp->cube_->render();
                break;
            case RenderType::RT_Quad:
                render_comp->quad_->render();
                break;
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 1);
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glViewport(0, 0, g_window_width_, g_window_height_);
    glClear(GL_DEPTH_BUFFER_BIT);

}
