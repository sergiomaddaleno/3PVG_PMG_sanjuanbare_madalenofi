
#include "Render/shadow_map.h"

#include "Systems/ecs_manager.h"
#include "Core/shader.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"
#include "global_data.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <memory> 
#include <iostream>

ShadowMap::ShadowMap() : depthMapFBO(0), depthMap(0), 
    near_plane(1.0f), far_plane(100.0f), directional_size_(50.0f),
    shadow_width_(4098), shadow_height_(4098)
{
}

ShadowMap::~ShadowMap()
{
    
}

void ShadowMap::init()
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

void ShadowMap::initCubeMap()
{
    char* vs_geometry = (char*)ReadFile("../assets/shaders/geometry.gs");
    GeomtryShader.compileGeometryShader(vs_geometry);

    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            shadow_width_, shadow_height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::active(unsigned int num)
{
    if (depthMap != 0) {
        glActiveTexture(GL_TEXTURE0 + num);
    }
}

void ShadowMap::bind()
{
    if (depthMap != 0) {
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }
}

void ShadowMap::renderSceneOneShadow(ECSManager& ecs, Shader shader, LightComponent lc) {

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    shader.useProgram();
    shader.setUniformValue(shader.getUniformLocation("u_light_space"), ValueType::T_MAT_4x4, glm::value_ptr(lightSpaceMatrix));

    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(lightView));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(lightProjection));

    int texture_unit = 0;
    shader.setUniformValue(shader.getUniformLocation("u_shadow_map"), ValueType::T_INT_1, &texture_unit);
    active();
    bind();

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
                for (auto& mesh : render_comp->mesh_->meshes) {
                    render_comp->mesh_->renderElements();
                }
                break;
            case RenderType::RT_Terrain:
                render_comp->terrain_->render();
                break;
            case RenderType::RT_Sphere:
                render_comp->sphere_->render();
                break;
            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v:
                render_comp->cube_->render();
                break;
            case RenderType::RT_Quad:
                render_comp->quad_->render();
                break;
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 1);
    glCullFace(GL_BACK);
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);
    glViewport(0, 0, global.window_width, global.window_height);
}

void ShadowMap::renderSceneMultipleShadow(ECSManager& ecs, Shader shader, Shader GeomtryShader,LightComponent lc) {

    glEnable(GL_DEPTH_TEST);
    shader.useProgram();

    shader.setUniformValue(shader.getUniformLocation("lightPos"), ValueType::T_FLOAT_3, glm::value_ptr(lc.light.pos_));

    shader.setUniformValue(shader.getUniformLocation("far_plane"), ValueType::T_FLOAT_1, &far_plane);

    // Vincular el cubemap de profundidad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    int texture_unit = 0;
    shader.setUniformValue(shader.getUniformLocation("depthMap"), ValueType::T_INT_1, &texture_unit);

    // Configurar framebuffer y viewport
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
                for (auto& mesh : render_comp->mesh_->meshes) {
                    render_comp->mesh_->renderElements();
                }
                break;
            case RenderType::RT_Terrain:
                render_comp->terrain_->render();
                break;
            case RenderType::RT_Sphere:
                render_comp->sphere_->render();
                break;
            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v:
                render_comp->cube_->render();
                break;
            case RenderType::RT_Quad:
                render_comp->quad_->render();
                break;
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, global.window_width, global.window_height);
}


void ShadowMap::render(ECSManager& ecs, Shader shader, LightComponent lc) {


    switch(lc.light.type_) {
    case LightType::LT_Spot: {

        float spotLightFOV = lc.light.spot_angle_ * 2.0f;
        lightProjection = glm::perspective(spotLightFOV, (float)global.window_width / (float)global.window_height, near_plane, far_plane);

        lightView = glm::lookAt(lc.light.pos_, lc.light.pos_ + lc.light.dir_, glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix = lightProjection * lightView;

        renderSceneOneShadow(ecs,shader,lc);

        break;
    }
    case LightType::LT_Directional:
    {
        lightProjection = glm::ortho(-directional_size_, directional_size_, -directional_size_, directional_size_, near_plane, far_plane);

        lightView = glm::lookAt(-lc.light.dir_ * 10.0f,glm::vec3(0.0f),glm::vec3(0.0f, 1.0f, 0.0f));

        lightSpaceMatrix = lightProjection * lightView;

        renderSceneOneShadow(ecs, shader, lc);

        break;
    }
    case LightType::LT_Point:
    {
    
        
        float aspect = (float) shadow_width_ / (float) shadow_height_;
        lightProjection = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);

        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(lightProjection * glm::lookAt(lc.light.pos_, lc.light.pos_ + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(lightProjection * glm::lookAt(lc.light.pos_, lc.light.pos_ + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(lightProjection * glm::lookAt(lc.light.pos_, lc.light.pos_ + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(lightProjection * glm::lookAt(lc.light.pos_, lc.light.pos_ + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(lightProjection * glm::lookAt(lc.light.pos_, lc.light.pos_ + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(lightProjection * glm::lookAt(lc.light.pos_, lc.light.pos_ + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        for (unsigned int i = 0; i < 6; i++)
        {
            GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, depthCubemap, 0);

            std::string uniformName = "shadowMatrices[" + std::to_string(i) + "]";
            GeomtryShader.setUniformValue(GeomtryShader.getUniformLocation(uniformName.c_str()), ValueType::T_MAT_4x4, glm::value_ptr(lightSpaceMatrix[i]));

            renderSceneMultipleShadow(ecs, shader, GeomtryShader,lc);
        }

        break;
    }
    }

   
}
