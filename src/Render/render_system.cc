

#include "Render/render_system.h"
#include "Core/mesh.h"
#include "Render/camera.h"
#include "Render/shadow_map.h"
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
            case RenderType::RT_SpotLight:
            case RenderType::RT_PointLight:
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

            auto li = ecs.getComponentList<LightComponent>().begin();
            auto ti = ecs.getComponentList<TransformComponent>().begin();
            auto liend = ecs.getComponentList<LightComponent>().end();
            auto tiend = ecs.getComponentList<TransformComponent>().end();


            for (; li != liend && ti != tiend; ++li, ++ti) {
                if (li->has_value() && ti->has_value()) {
                    auto& light_comp = *li;
                    auto& transform = *ti;

                    light_comp->shadow_map_.initMultipleShadow();
                    //light_comp->shadow_map_.initOneShadow();;
                }
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void RenderSystem::setEntitiesUniforms(TransformComponent tc, Shader shader, LightComponent light, Camera cam)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = 1;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), ValueType::T_INT_1, &tex_id);

    shader.setUniformValue(shader.getUniformLocation("u_fog"), ValueType::T_INT_1, &g_b_fog_);
    shader.setUniformValue(shader.getUniformLocation("u_density"), ValueType::T_FLOAT_1, &g_fog_density_);
    shader.setUniformValue(shader.getUniformLocation("u_gradient"), ValueType::T_FLOAT_1, &g_fog_gradient_);
    shader.setUniformValue(shader.getUniformLocation("u_sky_colour"), ValueType::T_FLOAT_3, &g_sky_colour_);
}

void RenderSystem::renderEntities(ECSManager& ecs, Shader shader, Camera cam) {

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    glEnable(GL_DEPTH_TEST);
    shader.useProgram();

    int light_count = 0;
    for (; li != liend && ti != tiend; ++li, ++ti) {
        if (li->has_value() && ti->has_value()) {
            auto& light_comp = *li;
            auto& transform = *ti;

            light_comp->shadow_map_.render(ecs, shader, cam, *light_comp);

            // Light ---------------------------------------------------------------
            shader.useProgram();
            shader.setUniformValue(shader.getUniformLocation("u_number_lights"), ValueType::T_INT_1, &number_lights);
            shader.setUniformValue(shader.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));
            shader.setUniformValue(shader.getUniformLocation("u_ambient"), ValueType::T_FLOAT_3, &light_comp->light.global_ambient_color_);

            light_comp->shadow_map_.bind();
            light_comp->shadow_map_.addNewShadow(light_count);
            int texture_unit = 0;
            shader.setUniformValue(shader.getUniformLocation("u_shadow_map"), ValueType::T_INT_1, &texture_unit);

            light_comp->light.setUniforms(shader, light_count);

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

                    setEntitiesUniforms(*transform_comp, shader, *light_comp, cam);

                    switch (render_comp->type_) {
                    case RenderType::RT_Mesh:
                    //case RenderType::RT_SpotLight:
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

size_t RenderSystem::createSpotLight(ECSManager& ecs, const Shader& light_shader, LightInfo li, bool default_light)
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

    if (default_light) {

        light_comp->light.light_info_.pos_ = glm::vec3(0.0f, 0.0f, 0.0f);
        light_comp->light.light_info_.dir_ = glm::vec3(0.0f, -1.0f, -1.0f);
        light_comp->light.light_info_.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
        light_comp->light.light_info_.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.light_info_.linear_att = 0.7f;
        light_comp->light.light_info_.quad_att_ = 1.8f;
        light_comp->light.light_info_.constant_att_ = 1.0f;
        light_comp->light.light_info_.shininess_ = 32.0f;
        light_comp->light.light_info_.strength_ = 1.0f;
        light_comp->light.light_info_.spot_angle_ = 45.0f;
    }
    else {
        light_comp->light.light_info_.pos_ = li.pos_;
        light_comp->light.light_info_.dir_ = li.dir_;
        light_comp->light.light_info_.light_color_ = li.light_color_;
        light_comp->light.light_info_.linear_att = li.linear_att;
        light_comp->light.light_info_.quad_att_ = li.quad_att_;
        light_comp->light.light_info_.constant_att_ = li.constant_att_;
        light_comp->light.light_info_.shininess_ = li.shininess_;
        light_comp->light.light_info_.strength_ = li.strength_;
        light_comp->light.light_info_.spot_angle_ = li.spot_angle_;
    }


    number_lights++;
    entity_++;

    //renderEntities(ecs, light_shader);
    

    return entity_;
}

size_t RenderSystem::createPointLight(ECSManager& ecs, Shader& light_shader, LightInfo li, bool default_light)
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

    if (default_light) {
        light_comp->light.light_info_.pos_ = li.pos_;
        light_comp->light.light_info_.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
        light_comp->light.light_info_.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.light_info_.linear_att = 0.7f;
        light_comp->light.light_info_.quad_att_ = 1.8f;
        light_comp->light.light_info_.constant_att_ = 1.0f;
        light_comp->light.light_info_.shininess_ = 32.0f;
        light_comp->light.light_info_.strength_ = 1.0f;
    }
    else {
        light_comp->light.light_info_.pos_ = li.pos_;
        light_comp->light.light_info_.light_color_ = li.light_color_;
        light_comp->light.light_info_.ambient_color_ = li.ambient_color_;
        light_comp->light.light_info_.linear_att = li.linear_att;
        light_comp->light.light_info_.quad_att_ = li.quad_att_;
        light_comp->light.light_info_.constant_att_ = li.constant_att_;
        light_comp->light.light_info_.shininess_ = li.shininess_;
        light_comp->light.light_info_.strength_ = li.strength_;
    }
    
    number_lights++;
    entity_++;
    //renderEntities(ecs, light_shader);
    return entity_;
}

size_t RenderSystem::createDirectionalLight(ECSManager& ecs, const Shader& light_shader, LightInfo li, bool default_light)
{
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    // ----

    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.light_info_.type_ = LightType::LT_Directional;
    light_comp->shader = light_shader;

  
    if (default_light) {
        light_comp->light.light_info_.dir_ = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
        light_comp->light.light_info_.light_color_ = glm::vec3(0.7f, 0.7f, 0.7f);
        light_comp->light.light_info_.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.light_info_.spec_color_ = glm::vec3(1.0f, 1.0f, 0.95f);
        light_comp->light.light_info_.linear_att = 0.0f;
        light_comp->light.light_info_.quad_att_ = 0.0f;
        light_comp->light.light_info_.constant_att_ = 1.0f;
        light_comp->light.light_info_.shininess_ = 34.0f;
    }else{
        light_comp->light.light_info_.dir_ = glm::normalize(li.dir_);
        light_comp->light.light_info_.light_color_ = li.light_color_;
        light_comp->light.light_info_.ambient_color_ = li.ambient_color_;
        light_comp->light.light_info_.spec_color_ = li.spec_color_;
        light_comp->light.light_info_.linear_att = li.linear_att;
        light_comp->light.light_info_.quad_att_ = li.quad_att_;
        light_comp->light.light_info_.constant_att_ = li.constant_att_;
        light_comp->light.light_info_.shininess_ = li.shininess_;
    }

    number_lights++;
    entity_++;
    
    return entity_;
}