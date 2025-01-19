

#include "..\include\render_system.h"
#include "..\include\mesh.h"
#include "utils.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"
#include "..\include\Components\light_component.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>


#include <../deps/glm/gtc/quaternion.hpp>


#include <memory> 

#include <iostream>

#define M_PI 3.14159265358979323846

RenderSystem::RenderSystem(){}
RenderSystem::~RenderSystem(){}

void RenderSystem::renderMesh(ECSManager& ecs, RenderData& rd) {

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

                glm::mat4 model = glm::translate(glm::mat4(1.0f), transform_comp->position) *
                    glm::mat4_cast(transform_comp->rotation) *
                    glm::scale(glm::mat4(1.0f), transform_comp->scale);

                glm::mat4 view = rd.cam.getViewMatrix();
                glm::mat4 projection = rd.cam.getProjectionMatrix();
                glm::mat4 mvp = projection * view * model;

                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                // Texture
                float tex_id = 0.0f;
                render_comp->texture = mesh.getTexture(0);
                render_comp->texture.bind();
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("texture1"), Type::T_INT_1, &tex_id);

                // Fog
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &rd.b_fog_);

                glBindVertexArray(mesh.VAO);
                glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
            }
        }
    }

    glUseProgram(0);
}


void RenderSystem::initEntities(ECSManager& ecs) {

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            for (auto& mesh : render_comp->mesh_->meshes) {

                glGenVertexArrays(1, &mesh.VAO);
                glBindVertexArray(mesh.VAO);

                glGenBuffers(1, &mesh.VBO);
                glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
                glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

                glGenBuffers(1, &mesh.EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
            }
        }
    }
}

void RenderSystem::renderEntities(ECSManager& ecs, RenderData& rd) {

    // -------------------------------------------------------------
    //Light

    auto li = ecs.getComponentList<LightComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    int counter = 0;
    for (; li != liend && ti != tiend; ++li, ++ti) {
        if (li->has_value() && ti->has_value()) {
            auto& light_comp = li->value();
            auto& transform = ti->value();

            light_comp.shader.useProgram();

            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation("u_number_lights"), Type::T_INT_1, &number_lights);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation("u_camera_position"), Type::T_FLOAT_3, glm::value_ptr(rd.cam.getPosition()));
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation("u_ambient"), Type::T_FLOAT_3, &global_ambient_color_);

            for (int i = 0; i < number_lights; ++i) {

                int loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].type_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_INT_1, &light_comp.light_info.type_);

                // ---

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].pos_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(transform.position));

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].dir_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp.light_info.dir_));

                //loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].diff_color_").c_str());
                //light_comp.shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp.light_info.diff_color_));

                //loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].spec_color_").c_str());
                //light_comp.shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp.light_info.spec_color_));

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].light_color_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp.light_info.light_color_));

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].ambient_color_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp.light_info.ambient_color_));

                // ---

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].linear_att_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp.light_info.linear_att);

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].quad_att_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp.light_info.quad_att_);

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].constant_att_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp.light_info.constant_att_);

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].shininess_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp.light_info.shininess_);

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].strength_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp.light_info.strength_);

                loc = light_comp.shader.getUniformLocation(("u_light[" + std::to_string(i) + "].spot_angle_").c_str());
                light_comp.shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp.light_info.spot_angle_);

            }

            // -------------------------------------------------------------
            //Mesh

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

                        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform_comp->position) *
                            glm::mat4_cast(transform_comp->rotation) *
                            glm::scale(glm::mat4(1.0f), transform_comp->scale);

                        glm::mat4 view = rd.cam.getViewMatrix();
                        glm::mat4 projection = rd.cam.getProjectionMatrix();
                        glm::mat4 mvp = projection * view * model;

                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                        // Texture
                        float tex_id = 0.0f;
                        render_comp->texture = mesh.getTexture(0);
                        render_comp->texture.bind();
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("texture"), Type::T_INT_1, &tex_id);
                        // ---

                        //Fog
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &rd.b_fog_);

                        glBindVertexArray(mesh.VAO);
                        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }
                }
            }
            glBlendFunc(GL_ONE, GL_ONE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

size_t RenderSystem::createMesh(ECSManager& ecs, const std::shared_ptr<Mesh>& entity_mesh,
    const Shader& mesh_shader
)
{
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    ecs.get_component<RenderComponent>(entity_)->shader = mesh_shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Mesh;
    ecs.get_component<RenderComponent>(entity_)->mesh_ = entity_mesh;

    entity_++;

    return entity_;
}

size_t RenderSystem::createSpotLight(ECSManager& ecs, const std::shared_ptr<Mesh>& light_bulb_mesh,
    const Shader& mesh_shader, const Shader& light_shader)
{
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    auto& render_comp = ecs.get_component<RenderComponent>(entity_);
    auto& transform_comp = ecs.get_component<TransformComponent>(entity_);
    auto& light_comp = ecs.get_component<LightComponent>(entity_);

    render_comp->shader = mesh_shader;
    render_comp->type_ = RenderType::RT_Light;
    render_comp->mesh_ = light_bulb_mesh;

    // ----

    light_comp->shader = light_shader;
    light_comp->light_info.type_ = LightType::LT_Spot;

    light_comp->light_info.dir_ = glm::vec3(0.0f, 0.0f, -1.0f);
    light_comp->light_info.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
    light_comp->light_info.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
    light_comp->light_info.linear_att = 0.7f;
    light_comp->light_info.quad_att_ = 1.8f;
    light_comp->light_info.constant_att_ = 1.0f;
    light_comp->light_info.shininess_ = 32.0f;
    light_comp->light_info.strength_ = 1.0f;
    light_comp->light_info.spot_angle_ = 45.0f;

    number_lights++;
    entity_++;

    return entity_;
}

size_t RenderSystem::createPointLight(ECSManager& ecs, const std::shared_ptr<Mesh>& light_bulb_mesh,
    const Shader& mesh_shader, const Shader& light_shader)
{
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    auto& render_comp = ecs.get_component<RenderComponent>(entity_);
    auto& transform_comp = ecs.get_component<TransformComponent>(entity_);
    auto& light_comp = ecs.get_component<LightComponent>(entity_);

    render_comp->shader = mesh_shader;
    render_comp->type_ = RenderType::RT_Light;
    render_comp->mesh_ = light_bulb_mesh;

    // ----

    light_comp->shader = light_shader;
    light_comp->light_info.type_ = LightType::LT_Point;

    light_comp->light_info.pos_ = transform_comp->position;
    light_comp->light_info.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
    light_comp->light_info.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
    light_comp->light_info.linear_att = 0.7f;
    light_comp->light_info.quad_att_ = 1.8f;
    light_comp->light_info.constant_att_ = 1.0f;
    light_comp->light_info.shininess_ = 32.0f;
    light_comp->light_info.strength_ = 1.0f;

    number_lights++;
    entity_++;

    return entity_;
}

size_t RenderSystem::createDirectionalLight(ECSManager& ecs, const std::shared_ptr<Mesh>& light_bulb_mesh,
    const Shader& mesh_shader,
    const Shader& light_shader)
{
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    auto& transform_comp = ecs.get_component<TransformComponent>(entity_);
    auto& light_comp = ecs.get_component<LightComponent>(entity_);

    // ----

    light_comp->shader = light_shader;
    light_comp->light_info.type_ = LightType::LT_Directional;

    light_comp->light_info.dir_ = glm::vec3(0.0f, -1.0f, 1.0f);
    light_comp->light_info.light_color_ = glm::vec3(4.0f, 4.0f, 0.0f);
    light_comp->light_info.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);

    light_comp->light_info.linear_att = 0.0f;
    light_comp->light_info.quad_att_ = 0.0f;
    light_comp->light_info.constant_att_ = 1.0f;

    number_lights++;
    entity_++;

    return entity_;
}










