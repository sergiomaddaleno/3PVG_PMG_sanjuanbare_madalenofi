

#include "Render/render_system.h"
#include "Core/mesh.h"
#include "Render/camera.h"
#include "Render/shadow_map.h"
#include "Core/shader.h"
#include "utils.h"
#include "global_data.h"
#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"
#include "Components\name_component.h"

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

RenderSystem::RenderSystem()
    : shader(),
    entity_(0),
    entity_mesh(0),
    entity_terrain(0),
    entity_sphere(0),
    entity_cube8v(0),
    entity_cube24v(0),
    entity_quad(0),
    entity_spotlight(0),
    entity_pointlight(0),
    entity_dirlight(0)
{}

RenderSystem::~RenderSystem() {}

void RenderSystem::init(ECSManager& ecs) {
    
    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto li = ecs.getComponentList<LightComponent>().begin();

    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto liend = ecs.getComponentList<LightComponent>().end();

    for (; ri != riend; ++ri) {
        if (ri->has_value()) {
            auto& render_comp = *ri;

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
                render_comp->mesh_->bind();
                break;

            case RenderType::RT_Terrain:
                render_comp->terrain_->bind();
                render_comp->terrain_->setTexture(render_comp->terrain_->texture_path_);
                break;

            case RenderType::RT_Sphere:
                render_comp->sphere_->bind();
                render_comp->sphere_->setTexture(render_comp->sphere_->texture_path_);
                break;

            case RenderType::RT_Quad:
                render_comp->quad_->bind();
                render_comp->quad_->setTexture(render_comp->quad_->texture_path_);
                break;

            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v:
                render_comp->cube_->bind();
                render_comp->cube_->setTexture(render_comp->cube_->texture_path_);
                break;
            }
            
            for (; li != liend; ++li) {
                if (li->has_value()) {
                    auto& light_comp = *li;

                    switch (light_comp->light.type_) {
                    case LightType::LT_Spot:
                    case LightType::LT_Directional:
                        light_comp->shadow_map_.init(); break;

                    case LightType::LT_Point:
                        light_comp->shadow_map_.initCubeMap(); break;
                    }
                }
            }
        }
    }
    
    deferred.init();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void RenderSystem::setUniforms(TransformComponent tc, Shader shader, LightComponent light, Camera cam)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = 1;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), ValueType::T_INT_1, &tex_id);

    shader.setUniformValue(shader.getUniformLocation("u_fog"), ValueType::T_INT_1, &global.use_fog);
    shader.setUniformValue(shader.getUniformLocation("u_density"), ValueType::T_FLOAT_1, &global.fog_density);
    shader.setUniformValue(shader.getUniformLocation("u_gradient"), ValueType::T_FLOAT_1, &global.fog_gradient);
    shader.setUniformValue(shader.getUniformLocation("u_sky_colour"), ValueType::T_FLOAT_3, &global.sky_colour);
}

void RenderSystem::render(ECSManager& ecs, Camera cam) {
    
    glEnable(GL_DEPTH_TEST);
    glClear( GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();

    shader.useProgram();

    for (; li != liend; ++li) {
        if (li->has_value()) {
            auto& light_comp = *li;

            light_comp->shadow_map_.render(ecs, shader, *light_comp);

            shader.setUniformValue(shader.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));
            
            light_comp->light.setUniforms(shader);
            
            // ---------------------------------------------------------------

            auto ri = ecs.getComponentList<RenderComponent>().begin();
            auto ti = ecs.getComponentList<TransformComponent>().begin();
            auto riend = ecs.getComponentList<RenderComponent>().end();
            auto tiend = ecs.getComponentList<TransformComponent>().end();

            for (; ri != riend && ti != tiend; ++ri, ++ti) {
                if (ri->has_value() && ti->has_value()) {
                    auto& render_comp = *ri;
                    auto& transform_comp = *ti;

                    setUniforms(*transform_comp, shader, *light_comp, cam);

                    switch (render_comp->type_) {
                    case RenderType::RT_Mesh:
                    {
                        int use_texture = render_comp->mesh_->use_texture_;
                        shader.setUniformValue(shader.getUniformLocation("u_use_texture"), ValueType::T_INT_1, &use_texture);
                        glm::vec3 color_tex = render_comp->mesh_->color_;
                        shader.setUniformValue(shader.getUniformLocation("u_color_texture"), ValueType::T_FLOAT_3, &color_tex);

                        for (auto& mesh : render_comp->mesh_->meshes) {
                            render_comp->mesh_->draw();
                        }
                    }
                    break;

                    case RenderType::RT_Terrain:
                    {
                        int use_texture = render_comp->terrain_->use_texture_;
                        shader.setUniformValue(shader.getUniformLocation("u_use_texture"), ValueType::T_INT_1, &use_texture);
                        glm::vec3 color_tex = render_comp->terrain_->color_;
                        shader.setUniformValue(shader.getUniformLocation("u_color_texture"), ValueType::T_FLOAT_3, &color_tex);

                        render_comp->terrain_->draw();

                        render_comp->terrain_->wave_.setUniforms(shader, (float)glfwGetTime());
                    }
                    break;

                    case RenderType::RT_Sphere:
                    {
                        int use_texture = render_comp->sphere_->use_texture_;
                        shader.setUniformValue(shader.getUniformLocation("u_use_texture"), ValueType::T_INT_1, &use_texture);
                        glm::vec3 color_tex = render_comp->sphere_->color_;
                        shader.setUniformValue(shader.getUniformLocation("u_color_texture"), ValueType::T_FLOAT_3, &color_tex);

                        render_comp->sphere_->draw();

                        //render_comp->sphere_->wave_.setUniforms(shader);
                    }
                    break;

                    case RenderType::RT_Cube8v:
                    case RenderType::RT_Cube24v:
                    {
                        int use_texture = render_comp->cube_->use_texture_;
                        shader.setUniformValue(shader.getUniformLocation("u_use_texture"), ValueType::T_INT_1, &use_texture);
                        glm::vec3 color_tex = render_comp->cube_->color_;
                        shader.setUniformValue(shader.getUniformLocation("u_color_texture"), ValueType::T_FLOAT_3, &color_tex);

                        render_comp->cube_->draw();

                        //render_comp->cube_->wave_.setUniforms(shader);
                    }
                    break;

                    case RenderType::RT_Quad:
                    {
                        int use_texture = render_comp->quad_->use_texture_;
                        shader.setUniformValue(shader.getUniformLocation("u_use_texture"), ValueType::T_INT_1, &use_texture);
                        glm::vec3 color_tex = render_comp->quad_->color_;
                        shader.setUniformValue(shader.getUniformLocation("u_color_texture"), ValueType::T_FLOAT_3, &color_tex);

                        render_comp->quad_->draw();

                        //render_comp->quad_->wave_.setUniforms(shader);
                    }
                    break;
                    }
                }
            }
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }
    
    glDisable(GL_BLEND);
    glUseProgram(0);
    
     //deferred.resize(global.window_width, global.window_height);
     //deferred.render(ecs, cam);

}

void RenderSystem::setup(ECSManager& ecs)
{
    entity_ = ecs.create_entity();

    ecs.add_component_type<NameComponent>();
    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();
    ecs.add_component_type<LightComponent>();
}

size_t RenderSystem::createMesh(ECSManager& ecs, std::string file, std::string textureFolder, 
    TransformComponent tc)
{
    auto mesh = std::make_unique<Mesh>();
    if (!mesh->loadOBJ(file, textureFolder)) {
        return -1;
    }
    
    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    ecs.get_component<NameComponent>(entity_)->name = "Mesh [" + std::to_string(entity_mesh) + "]";

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Mesh;
    ecs.get_component<RenderComponent>(entity_)->mesh_ = std::move(mesh);

    entity_++;
    entity_mesh++;

    return entity_;
}

size_t RenderSystem::createTerrain(ECSManager& ecs, TransformComponent tc,
    const int num_cols, const int num_rows,
    const float height_mult, const float size, 
    const bool is_centered, const bool use_perlin_noise){

    auto terrain = std::make_unique<Terrain>();
    terrain->init(num_cols, num_rows, height_mult, size, is_centered, use_perlin_noise);

    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---
    ecs.get_component<NameComponent>(entity_)->name = "Terrain [" + std::to_string(entity_terrain) + "]";

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Terrain;
    ecs.get_component<RenderComponent>(entity_)->terrain_ = std::move(terrain);

    entity_++;
    entity_terrain++;

    return entity_;
}

size_t RenderSystem::createSphere(ECSManager& ecs, TransformComponent tc, int num_heights_, int num_revs_, float size)
{
    auto sphere = std::make_unique<Sphere>();
    sphere->init(size, num_heights_, num_revs_);

    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---

    ecs.get_component<NameComponent>(entity_)->name = "Sphere [" + std::to_string(entity_sphere) + "]";

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Sphere;
    ecs.get_component<RenderComponent>(entity_)->sphere_ = std::move(sphere);

    entity_++;
    entity_sphere++;

    return entity_;
}

size_t RenderSystem::createCube8v(ECSManager& ecs, TransformComponent tc, const float size)
{
    auto cube = std::make_unique<Cube>();
    cube->init8v(size);

    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---
    ecs.get_component<NameComponent>(entity_)->name = "Cube8v [" + std::to_string(entity_cube8v) + "]";

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube8v;
    ecs.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);

    entity_++;
    entity_cube8v++;

    return entity_;
}

size_t RenderSystem::createCube24v(ECSManager& ecs, TransformComponent tc, const float size)
{
    auto cube = std::make_unique<Cube>();
    cube->init24v(size);

    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---
    ecs.get_component<NameComponent>(entity_)->name = "Cube24v [" + std::to_string(entity_cube24v) + "]";

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube24v;
    ecs.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);

    entity_++;
    entity_cube24v++;

    return entity_;
}

size_t RenderSystem::createQuad(ECSManager& ecs, TransformComponent tc, const float size)
{
    auto quad = std::make_unique<Quad>();
    quad->init(size);

    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    //---
    ecs.get_component<NameComponent>(entity_)->name = "Quad [" + std::to_string(entity_quad) + "]";

    auto& transform = ecs.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Quad;
    ecs.get_component<RenderComponent>(entity_)->quad_ = std::move(quad);

    entity_++;
    entity_quad++;

    return entity_;

}

size_t RenderSystem::createSpotLight(ECSManager& ecs, Light li, bool default_light)
{
    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    //---
    ecs.get_component<NameComponent>(entity_)->name = "Spot Light [" + std::to_string(entity_spotlight) + "]";

    ecs.add_component<TransformComponent>(entity_)->position = li.pos_;

    // ----
    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.type_ = LightType::LT_Spot;

    if (default_light) {

        light_comp->light.pos_ = glm::vec3(0.0f, 0.0f, 0.0f);
        light_comp->light.dir_ = glm::vec3(0.0f, -1.0f, -1.0f);
        light_comp->light.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
        light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.linear_att = 0.7f;
        light_comp->light.quad_att_ = 1.8f;
        light_comp->light.constant_att_ = 1.0f;
        light_comp->light.shininess_ = 32.0f;
        light_comp->light.strength_ = 1.0f;
        light_comp->light.spot_angle_ = 45.0f;
        light_comp->light.spot_softness_ = 10.0f;
    }
    else {
        light_comp->light.pos_ = li.pos_;
        light_comp->light.dir_ = li.dir_;
        light_comp->light.light_color_ = li.light_color_;
        light_comp->light.linear_att = li.linear_att;
        light_comp->light.quad_att_ = li.quad_att_;
        light_comp->light.constant_att_ = li.constant_att_;
        light_comp->light.shininess_ = li.shininess_;
        light_comp->light.strength_ = li.strength_;
        light_comp->light.spot_angle_ = li.spot_angle_;
        light_comp->light.spot_softness_ = li.spot_softness_;
    }

    entity_++;
    entity_spotlight++;

    return entity_;
}

size_t RenderSystem::createPointLight(ECSManager& ecs, Light li, bool default_light)
{
    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    //---
    ecs.get_component<NameComponent>(entity_)->name = "Point Light [" + std::to_string(entity_spotlight) + "]";

    ecs.get_component<TransformComponent>(entity_)->position = li.pos_;
    //--

    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.type_ = LightType::LT_Point;

    if (default_light) {
        light_comp->light.pos_ = li.pos_;
        light_comp->light.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
        light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.linear_att = 0.7f;
        light_comp->light.quad_att_ = 1.8f;
        light_comp->light.constant_att_ = 1.0f;
        light_comp->light.shininess_ = 32.0f;
        light_comp->light.strength_ = 1.0f;
    }
    else {
        light_comp->light.pos_ = li.pos_;
        light_comp->light.light_color_ = li.light_color_;
        light_comp->light.ambient_color_ = li.ambient_color_;
        light_comp->light.linear_att = li.linear_att;
        light_comp->light.quad_att_ = li.quad_att_;
        light_comp->light.constant_att_ = li.constant_att_;
        light_comp->light.shininess_ = li.shininess_;
        light_comp->light.strength_ = li.strength_;
    }
    
    entity_++;
    entity_pointlight++;

    return entity_;
}

size_t RenderSystem::createDirectionalLight(ECSManager& ecs, Light li, bool default_light)
{
    ecs.add_component<NameComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    // ----
    ecs.get_component<NameComponent>(entity_)->name = "Directional Light [" + std::to_string(entity_dirlight) + "]";

    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    light_comp->light.type_ = LightType::LT_Directional;
  
    if (default_light) {
        light_comp->light.dir_ = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
        light_comp->light.light_color_ = glm::vec3(0.7f, 0.7f, 0.7f);
        light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.spec_color_ = glm::vec3(1.0f, 1.0f, 0.95f);
        light_comp->light.linear_att = 0.0f;
        light_comp->light.quad_att_ = 0.0f;
        light_comp->light.constant_att_ = 1.0f;
        light_comp->light.shininess_ = 34.0f;
    }else{
        light_comp->light.dir_ = glm::normalize(li.dir_);
        light_comp->light.light_color_ = li.light_color_;
        light_comp->light.ambient_color_ = li.ambient_color_;
        light_comp->light.spec_color_ = li.spec_color_;
        light_comp->light.linear_att = li.linear_att;
        light_comp->light.quad_att_ = li.quad_att_;
        light_comp->light.constant_att_ = li.constant_att_;
        light_comp->light.shininess_ = li.shininess_;
    }

    entity_++;
    entity_dirlight++;
    
    return entity_;
}