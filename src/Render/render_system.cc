

#include "Render/render_system.h"
#include "Core/mesh.h"
#include "Render/camera.h"
#include "Core/shader.h"
#include "utils.h"
#include "global_variables.h"
#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <memory> 
#include <iostream>

#define M_PI 3.14159265358979323846

RenderSystem::RenderSystem() {}
RenderSystem::~RenderSystem() {}

char* fs_depthmap = (char*)ReadFile("../assets/shaders/depth_map.fs");
char* vs_depthmap = (char*)ReadFile("../assets/shaders/depth_map.vs");

char* vs_entities2 = (char*)ReadFile("../assets/shaders/entities.vs");
char* fs_entities2 = (char*)ReadFile("../assets/shaders/entities.fs");

Shader depth_map, entities_shader2;

unsigned int depthMapFBO;
unsigned int depthMap;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

void RenderSystem::initEntities(ECSManager& ecs) {
    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto li = ecs.getComponentList<LightComponent>().begin();

    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();
    auto liend = ecs.getComponentList<LightComponent>().end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
            case RenderType::RT_PointLight:
            case RenderType::RT_SpotLight:
                render_comp->mesh_->bind();
                break;

            case RenderType::RT_Terrain:
                render_comp->terrain_->bind();
                break;

            case RenderType::RT_Sphere:
                render_comp->sphere_->bind();
                break;

            case RenderType::RT_Quad:
                render_comp->quad_->bind();
                break;

            case RenderType::RT_Cube:
                render_comp->cube_->bind();
                break;
            }

            glGenFramebuffers(1, &depthMapFBO);
            glGenTextures(1, &depthMap);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    depth_map.compile(fs_depthmap, vs_depthmap);
    entities_shader2.compile(fs_entities2, vs_entities2);

    glUseProgram(0);
}

void RenderSystem::renderShadowMap(ECSManager& ecs, Shader shader) {
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    float near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    glm::mat4 model = glm::mat4(1.0f);

    shader.useProgram();
    shader.setUniformValue(shader.getUniformLocation("u_light_space"), Type::T_MAT_4x4, glm::value_ptr(lightSpaceMatrix));
    shader.setUniformValue(shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(lightView));
    shader.setUniformValue(shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(lightProjection));
    shader.setUniformValue(shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
    int one = 1;
    shader.setUniformValue(shader.getUniformLocation("u_shadow_map"), Type::T_INT_1, &one);

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

            Camera cam;
            setEntitiesUniforms(*transform_comp, render_comp->shader, cam);

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
            case RenderType::RT_SpotLight:
            case RenderType::RT_PointLight:
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
            case RenderType::RT_Cube:
                render_comp->cube_->render();
                break;
            case RenderType::RT_Quad:
                render_comp->quad_->render();
                break;
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void RenderSystem::setEntitiesUniforms(TransformComponent tc, Shader shader, Camera cam)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();
    
    shader.setUniformValue(shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = 0;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), Type::T_INT_1, &tex_id);

    shader.setUniformValue(shader.getUniformLocation("u_fog"), Type::T_INT_1, &g_b_fog_);
    shader.setUniformValue(shader.getUniformLocation("u_density"), Type::T_FLOAT_1, &g_fog_density_);
    shader.setUniformValue(shader.getUniformLocation("u_gradient"), Type::T_FLOAT_1, &g_fog_gradient_);
    shader.setUniformValue(shader.getUniformLocation("u_sky_colour"), Type::T_FLOAT_3, &g_sky_colour_);
}

void RenderSystem::renderEntities(ECSManager& ecs, Camera cam) {

    //renderShadowMap(ecs);

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    int light_count = 0;
    for (; li != liend && ti != tiend; ++li, ++ti) {
        if (li->has_value() && ti->has_value()) {
            auto& light_comp = *li;
            auto& transform = *ti;

            // Light ---------------------------------------------------------------
            light_comp->shader.useProgram();

            light_comp->shader.setUniformValue(light_comp->shader.getUniformLocation("u_number_lights"), Type::T_INT_1, &number_lights);
            light_comp->shader.setUniformValue(light_comp->shader.getUniformLocation("u_camera_position"), Type::T_FLOAT_3, glm::value_ptr(cam.getPosition()));
            light_comp->shader.setUniformValue(light_comp->shader.getUniformLocation("u_ambient"), Type::T_FLOAT_3, &light_comp->light.global_ambient_color_);

            light_comp->light.setUniforms(light_comp->shader, light_count);

            light_count++;

            // ---------------------------------------------------------------

            auto ri = ecs.getComponentList<RenderComponent>().begin();
            auto ti = ecs.getComponentList<TransformComponent>().begin();
            auto riend = ecs.getComponentList<RenderComponent>().end();
            auto tiend = ecs.getComponentList<TransformComponent>().end();

            for (; ri != riend && ti != tiend; ++ri, ++ti) {
                if (ri->has_value() && ti->has_value()) {
                    auto& render_comp = *ri;
                    auto& transform_comp = *ti;

                    glCullFace(GL_BACK);
                    render_comp->shader.useProgram();

                    setEntitiesUniforms(*transform_comp, render_comp->shader, cam);

                    switch (render_comp->type_) {
                    case RenderType::RT_Mesh:
                    case RenderType::RT_SpotLight:
                    case RenderType::RT_PointLight:
                        for (auto& mesh : render_comp->mesh_->meshes) {
                            render_comp->mesh_->renderElements();
                        }
                        break;
                    case RenderType::RT_Terrain: {
                        render_comp->terrain_->render();
                        break;
                    }
                    case RenderType::RT_Sphere: {
                        render_comp->sphere_->render();
                        break;
                    }
                    case RenderType::RT_Cube: {
                        render_comp->cube_->render();
                        break;
                    }
                    case RenderType::RT_Quad: {
                        render_comp->quad_->render();
                        break;
                    }
                    }
                }
            }
        }
    }
    glUseProgram(0);
}

void RenderSystem::renderMesh(ECSManager& ecs, Camera cam)
{
    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            for (auto& mesh : render_comp->mesh_->meshes) {
                render_comp->shader.useProgram();
                glCullFace(GL_BACK);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform_comp->position);
                model = model * glm::mat4_cast(transform_comp->rotation);
                model = glm::scale(model, transform_comp->scale);

                glm::mat4 view = cam.getViewMatrix();
                glm::mat4 projection = cam.getProjectionMatrix();

                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                // Texture
                float tex_id = 0.0f;
                render_comp->texture = mesh.getTexture(0);
                render_comp->texture.bind();
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("texture1"), Type::T_INT_1, &tex_id);

                // Fog
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &g_b_fog_);

                glBindVertexArray(mesh.VAO);
                glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }

    glUseProgram(0);
}

void RenderSystem::setupEntities(ECSManager& ecs)
{
    entity_ = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();
    ecs.add_component_type<LightComponent>();
}

size_t RenderSystem::createMesh(ECSManager& ecs, Shader& mesh_shader, std::string file, std::string textureFolder, 
    TransformComponent tc)
{
    auto mario_mesh = std::make_unique<Mesh>();
    if (!mario_mesh->loadOBJ(file, textureFolder)) {
        return -1;
    }
    
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Mesh;
    ecs.get_component<RenderComponent>(entity_)->mesh_ = std::move(mario_mesh);
    ecs.get_component<RenderComponent>(entity_)->shader = mesh_shader;

    entity_++;

    return entity_;
}

size_t RenderSystem::createTerrain(ECSManager& ecs, Shader& shader, 
    TransformComponent tc,
    const int num_cols, const int num_rows,
    const float height_mult, const float size, 
    const bool is_centered, const bool use_perlin_noise){

    auto terrain = std::make_unique<Terrain>();
    terrain->init(num_cols, num_rows, height_mult, size, is_centered, use_perlin_noise);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Terrain;
    ecs.get_component<RenderComponent>(entity_)->terrain_ = std::move(terrain);
    ecs.get_component<RenderComponent>(entity_)->shader = shader;

    entity_++;

    return entity_;
}

size_t RenderSystem::createSphere(ECSManager& ecs, Shader& shader, TransformComponent tc, int num_heights_, int num_revs_, float size)
{
    auto sphere = std::make_unique<Sphere>();
    sphere->init(false, size, num_heights_, num_revs_);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Sphere;
    ecs.get_component<RenderComponent>(entity_)->sphere_ = std::move(sphere);
    ecs.get_component<RenderComponent>(entity_)->shader = shader;

    entity_++;

    return entity_;
}

size_t RenderSystem::createCube8v(ECSManager& ecs, Shader& shader, TransformComponent tc, const float size)
{
    auto cube = std::make_unique<Cube>();
    cube->init8v(size);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube;
    ecs.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);
    ecs.get_component<RenderComponent>(entity_)->shader = shader;

    entity_++;

    return entity_;
}

size_t RenderSystem::createCube24v(ECSManager& ecs, Shader& shader, TransformComponent tc, const float size)
{
    auto cube = std::make_unique<Cube>();
    cube->init24v(size);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->shader = shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube;
    ecs.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);

    entity_++;

    return entity_;
}

size_t RenderSystem::createQuad(ECSManager& ecs, Shader& shader, TransformComponent tc, const float size)
{
    auto quad = std::make_unique<Quad>();
    quad->init(size);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Quad;
    ecs.get_component<RenderComponent>(entity_)->quad_ = std::move(quad);
    ecs.get_component<RenderComponent>(entity_)->shader = shader;

    entity_++;
    return entity_;

}

size_t RenderSystem::createSpotLight(ECSManager& ecs, const Shader& light_shader, LightInfo li)
{
    auto lb_mesh = std::make_unique<Mesh>();
    lb_mesh->loadOBJ("../assets/obj/light_bulb.obj");

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    //---

    ecs.add_component<TransformComponent>(entity_)->position = li.pos_;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_SpotLight;
    ecs.get_component<RenderComponent>(entity_)->mesh_ = std::move(lb_mesh);
    ecs.get_component<RenderComponent>(entity_)->shader = light_shader;

    // ----
    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.light_info_.type_ = LightType::LT_Spot;
    light_comp->shader = light_shader;
    light_comp->light.light_info_.pos_ = li.pos_;
    light_comp->light.light_info_.dir_ = glm::vec3(0.0f, 0.0f, -1.0f);
    light_comp->light.light_info_.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
    light_comp->light.light_info_.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
    light_comp->light.light_info_.linear_att = 0.7f;
    light_comp->light.light_info_.quad_att_ = 1.8f;
    light_comp->light.light_info_.constant_att_ = 1.0f;
    light_comp->light.light_info_.shininess_ = 32.0f;
    light_comp->light.light_info_.strength_ = 1.0f;
    light_comp->light.light_info_.spot_angle_ = 20.0f;

    number_lights++;
    entity_++;

    return entity_;
}

size_t RenderSystem::createPointLight(ECSManager& ecs, Shader& light_shader, LightInfo li)
{
    auto lb_mesh = std::make_unique<Mesh>();
    lb_mesh->loadOBJ("../assets/obj/light_bulb.obj");

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    //---

    ecs.get_component<TransformComponent>(entity_)->position = li.pos_;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_PointLight;
    ecs.get_component<RenderComponent>(entity_)->mesh_ = std::move(lb_mesh);
    ecs.get_component<RenderComponent>(entity_)->shader = light_shader;

    //--

    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.light_info_.type_ = LightType::LT_Point;
    light_comp->shader = light_shader;
    light_comp->light.light_info_.pos_ = li.pos_;
    light_comp->light.light_info_.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
    light_comp->light.light_info_.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
    light_comp->light.light_info_.linear_att = 0.7f;
    light_comp->light.light_info_.quad_att_ = 1.8f;
    light_comp->light.light_info_.constant_att_ = 1.0f;
    light_comp->light.light_info_.shininess_ = 32.0f;
    light_comp->light.light_info_.strength_ = 1.0f;

    number_lights++;
    entity_++;

    return entity_;
}

size_t RenderSystem::createDirectionalLight(ECSManager& ecs, const Shader& light_shader, LightInfo li)
{
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    // ----

    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.light_info_.type_ = LightType::LT_Directional;
    light_comp->shader = light_shader;

    light_comp->light.light_info_.dir_ = glm::normalize(glm::vec3(-1.0f, -1.0f, -0.5f));
    light_comp->light.light_info_.light_color_ = glm::vec3(1.0f, 1.0f, 0.95f);
    light_comp->light.light_info_.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
    light_comp->light.light_info_.spec_color_ = glm::vec3(1.0f, 1.0f, 0.95f);
    light_comp->light.light_info_.linear_att = 0.0f;
    light_comp->light.light_info_.quad_att_ = 0.0f;
    light_comp->light.light_info_.constant_att_ = 1.0f;
    light_comp->light.light_info_.shininess_ = 32.0f;

    number_lights++;
    entity_++;

    return entity_;
}