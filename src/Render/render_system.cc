

#include "Render/render_system.h"
#include "Render/mesh.h"
#include "Render/camera.h"
#include "Render/shader.h"



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

RenderSystem::RenderSystem(){}
RenderSystem::~RenderSystem(){}

void RenderSystem::initEntities(ECSManager& ecs) {
    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            if (render_comp->type_ == RenderType::RT_Mesh) {
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

            if (render_comp->type_ == RenderType::RT_Terrain) {

                auto& terrain = render_comp->terrain_;

                glGenBuffers(1, &terrain->VBO);
                glBindBuffer(GL_ARRAY_BUFFER, terrain->VBO);
                glBufferData(GL_ARRAY_BUFFER, terrain->data_.size() * sizeof(float), terrain->data_.data(), GL_STATIC_DRAW);

                glGenBuffers(1, &terrain->EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrain->order_.size() * sizeof(int), terrain->order_.data(), GL_STATIC_DRAW);

                glGenVertexArrays(1, &terrain->VAO);
                glBindVertexArray(terrain->VAO);
                glBindBuffer(GL_ARRAY_BUFFER, terrain->VBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->EBO);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
            }

            if (render_comp->type_ == RenderType::RT_Sphere) {

                auto& sphere = render_comp->sphere_;

                glGenBuffers(1, &sphere->VBO);
                glBindBuffer(GL_ARRAY_BUFFER, sphere->VBO);
                glBufferData(GL_ARRAY_BUFFER, sphere->data_.size() * sizeof(float), sphere->data_.data(), GL_STATIC_DRAW);

                glGenBuffers(1, &sphere->EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere->order_.size() * sizeof(int), sphere->order_.data(), GL_STATIC_DRAW);

                glGenVertexArrays(1, &sphere->VAO);
                glBindVertexArray(sphere->VAO);
                glBindBuffer(GL_ARRAY_BUFFER, sphere->VBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->EBO);


                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
            }

            if (render_comp->type_ == RenderType::RT_Quad) {

                auto& quad = render_comp->quad_;

                glGenVertexArrays(1, &quad->VAO);
                glGenBuffers(1, &quad->VBO);
                glGenBuffers(1, &quad->EBO);

                glBindVertexArray(quad->VAO);

                glBindBuffer(GL_ARRAY_BUFFER, quad->VBO);
                glBufferData(GL_ARRAY_BUFFER, quad->data_.size() * sizeof(float), quad->data_.data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad->order_.size() * sizeof(int), quad->order_.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);
            }

            if (render_comp->type_ == RenderType::RT_Cube) {


                auto& cube = render_comp->cube_;
                
                glGenVertexArrays(1, &cube->VAO);
                glGenBuffers(1, &cube->VBO);
                glGenBuffers(1, &cube->EBO);

                glBindVertexArray(cube->VAO);

                glBindBuffer(GL_ARRAY_BUFFER, cube->VBO);
                glBufferData(GL_ARRAY_BUFFER, cube->data_.size() * sizeof(float), cube->data_.data(), GL_STATIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube->order_.size() * sizeof(int), cube->order_.data(), GL_STATIC_DRAW);

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
                glEnableVertexAttribArray(2);

                glBindVertexArray(0);

            }




           
        }
    }
    glUseProgram(0);
}

void RenderSystem::renderEntities(ECSManager& ecs, Camera cam) {
    
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
            light_comp->shader.setUniformValue(light_comp->shader.getUniformLocation("u_ambient"), Type::T_FLOAT_3, &global_ambient_color_);

            int loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].type_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_INT_1, &light_comp->light_info.type_);

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].pos_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(transform->position));

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].dir_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp->light_info.dir_));

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].light_color_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp->light_info.light_color_));

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].ambient_color_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_3, glm::value_ptr(light_comp->light_info.ambient_color_));

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].linear_att_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp->light_info.linear_att);

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].quad_att_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp->light_info.quad_att_);

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].constant_att_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp->light_info.constant_att_);

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].shininess_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp->light_info.shininess_);

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].strength_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp->light_info.strength_);

            loc = light_comp->shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].spot_angle_").c_str());
            light_comp->shader.setUniformValue(loc, Type::T_FLOAT_1, &light_comp->light_info.spot_angle_);


            // Mesh ---------------------------------------------------------------

            auto ri = ecs.getComponentList<RenderComponent>().begin();
            auto ti = ecs.getComponentList<TransformComponent>().begin();
            auto riend = ecs.getComponentList<RenderComponent>().end();
            auto tiend = ecs.getComponentList<TransformComponent>().end();

            for (; ri != riend && ti != tiend; ++ri, ++ti) {
                if (ri->has_value() && ti->has_value()) {
                    auto& render_comp = *ri;
                    auto& transform_comp = *ti;

                    if (render_comp->type_ == RenderType::RT_Mesh || render_comp->type_ == RenderType::RT_Light) {
                        for (auto& mesh : render_comp->mesh_->meshes) {
                            render_comp->shader.useProgram();
                            glCullFace(GL_BACK);

                            glm::mat4 model = glm::mat4(1.0f);
                            model = glm::translate(model, transform_comp->position);
                            model = model * glm::mat4_cast(transform_comp->rotation);
                            model = glm::scale(model, transform_comp->scale);

                            glm::mat4 view = cam.getViewMatrix();
                            glm::mat4 projection = cam.getProjectionMatrix();
                            glm::mat4 mvp = projection * view * model;

                            render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                            render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                            render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                            render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                            //Texture
                            render_comp->texture = mesh.getTexture(0);
                            render_comp->texture.bind();
                            float tex_id = 0.0f;
                            render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("texture_"), Type::T_INT_1, &tex_id);

                            //Fog
                            render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &b_fog_);

                            glBindVertexArray(mesh.VAO);
                            glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
                            glBindVertexArray(0);
                        }
                    }

                    // Terrain ---------------------------------------------------------------

                    if (render_comp->type_ == RenderType::RT_Terrain) {
                        

                        auto& terrain = render_comp->terrain_;

                        render_comp->shader.useProgram();
                        glCullFace(GL_BACK);

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, transform_comp->position);
                        model = model * glm::mat4_cast(transform_comp->rotation);
                        model = glm::scale(model, transform_comp->scale);

                        glm::mat4 view = cam.getViewMatrix();
                        glm::mat4 projection = cam.getProjectionMatrix();
                        glm::mat4 mvp = projection * view * model;
                        
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                        //Fog
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &b_fog_);

                        glBindVertexArray(terrain->VAO);
                        glDrawElements(GL_TRIANGLES, (terrain->num_cols_ * terrain->num_rows_) * 6, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }


                    if (render_comp->type_ == RenderType::RT_Sphere) {
                       
                        auto& sphere = render_comp->sphere_;

                        if (!sphere) {
                            std::cout << "Error: Sphere pointer is NULL!" << std::endl;
                            continue; // Saltar la renderización si el puntero es nulo
                        }

                        render_comp->shader.useProgram();
                        glCullFace(GL_BACK);

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, transform_comp->position);
                        model = model * glm::mat4_cast(transform_comp->rotation);
                        model = glm::scale(model, transform_comp->scale);

                        glm::mat4 view = cam.getViewMatrix();
                        glm::mat4 projection = cam.getProjectionMatrix();
                        glm::mat4 mvp = projection * view * model;

                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                        //Fog
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &b_fog_);

                        glBindVertexArray(sphere->VAO);
                        glDrawElements(GL_TRIANGLES, (sphere->num_heights_ * sphere->num_revs_) * 6, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }

                    if (render_comp->type_ == RenderType::RT_Cube) {
                        

                        auto& cube = render_comp->cube_;

                        render_comp->shader.useProgram();
                        glCullFace(GL_BACK);

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, transform_comp->position);
                        model = model * glm::mat4_cast(transform_comp->rotation);
                        model = glm::scale(model, transform_comp->scale);

                        glm::mat4 view = cam.getViewMatrix();
                        glm::mat4 projection = cam.getProjectionMatrix();
                        glm::mat4 mvp = projection * view * model;

                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                        //Fog
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &b_fog_);

                        glBindVertexArray(cube->VAO);
                        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }

                    if (render_comp->type_ == RenderType::RT_Quad) {
                       

                        auto& quad = render_comp->quad_;

                        render_comp->shader.useProgram();
                        glCullFace(GL_BACK);

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, transform_comp->position);
                        model = model * glm::mat4_cast(transform_comp->rotation);
                        model = glm::scale(model, transform_comp->scale);

                        glm::mat4 view = cam.getViewMatrix();
                        glm::mat4 projection = cam.getProjectionMatrix();
                        glm::mat4 mvp = projection * view * model;

                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

                        //Fog
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &b_fog_);

                        glBindVertexArray(quad->VAO);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }


                }
            }
            light_count++;
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
                render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &b_fog_);

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

size_t RenderSystem::createMesh(ECSManager& ecs, const std::shared_ptr<Mesh>& entity_mesh, Shader& mesh_shader
)
{

    //entity_ = ecs.create_entity();
    auto mario_mesh = std::make_unique<Mesh>();

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
   
    ecs.get_component<RenderComponent>(entity_)->shader = mesh_shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Mesh;
    auto light = std::make_unique<bool>(false);
    ecs.get_component<RenderComponent>(entity_)->light = std::move(light);
    ecs.get_component<RenderComponent>(entity_)->mesh_ = std::move(mario_mesh);

    entity_++;

    return entity_;
}

size_t RenderSystem::createTerrain(ECSManager& ecs, Shader& shader, int cols , int rows)
{
   

    auto terrain = std::make_unique<Terrain>();
    terrain->init(cols, rows, 1.0f, 1.0f, true, true);
   
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    ecs.get_component<RenderComponent>(entity_)->shader = shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Terrain;
    ecs.get_component<RenderComponent>(entity_)->terrain_ = std::move(terrain);
    auto light = std::make_unique<bool>(false);
    ecs.get_component<RenderComponent>(entity_)->light = std::move(light);
    ecs.add_component<TransformComponent>(entity_)->position = glm::vec3(0.0f, -5.0f, 0.0f);

   
    entity_++;

    return entity_;
}


size_t RenderSystem::createSphere(ECSManager& ecs, Shader& shader, int num_heights_, int num_revs_,float size)
{

   

    auto sphere = std::make_unique<Sphere>();
    sphere->init(false,size ,num_heights_,num_revs_);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    ecs.get_component<RenderComponent>(entity_)->shader = shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Sphere;
    ecs.get_component<RenderComponent>(entity_)->sphere_ = std::move(sphere);
    auto light = std::make_unique<bool>(false);
    ecs.get_component<RenderComponent>(entity_)->light = std::move(light);
    auto renderComp = ecs.get_component<RenderComponent>(entity_);
    if (renderComp && renderComp->sphere_) {
        std::cout << "Sphere pointer valid" << std::endl;
    }
    else {
        std::cout << "Sphere pointer is NULL!" << std::endl;
    }


    ecs.add_component<TransformComponent>(entity_)->position = glm::vec3(0.0f, -5.0f, 0.0f);

    
    entity_++;

    return entity_;
}

size_t RenderSystem::createCube8v(ECSManager& ecs, Shader& shader, const float size)
{

    
    auto cube = std::make_unique<Cube>();
    cube->init8v(size);
   
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    ecs.get_component<RenderComponent>(entity_)->shader = shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube;
   ecs.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);
   auto light = std::make_unique<bool>(false);
   ecs.get_component<RenderComponent>(entity_)->light = std::move(light);
    ecs.add_component<TransformComponent>(entity_)->position = glm::vec3(0.0f, -5.0f, 0.0f);

    
    entity_++;

    return entity_;
}

size_t RenderSystem::createCube24v(ECSManager& ecs, Shader& shader, const float size)
{
   

    auto cube = std::make_unique<Cube>();
    cube->init24v(size);
   
    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    ecs.get_component<RenderComponent>(entity_)->shader = shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube;
    ecs.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);
    auto light = std::make_unique<bool>(false);
    ecs.get_component<RenderComponent>(entity_)->light = std::move(light);
    ecs.add_component<TransformComponent>(entity_)->position = glm::vec3(0.0f, -5.0f, 0.0f);

    entity_++;

    return entity_;
}

size_t RenderSystem::createQuad(ECSManager& ecs, Shader& shader, const float size)
{

    

    auto quad = std::make_unique<Quad>();
    quad->init(size);

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);

    ecs.get_component<RenderComponent>(entity_)->shader = shader;
    ecs.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Quad;
    ecs.get_component<RenderComponent>(entity_)->quad_ = std::move(quad);

    auto light = std::make_unique<bool>(false);
    ecs.get_component<RenderComponent>(entity_)->light = std::move(light);
    ecs.add_component<TransformComponent>(entity_)->position = glm::vec3(0.0f, -5.0f, 0.0f);


    entity_ ++;
    return entity_;
    
}


size_t RenderSystem::createSpotLight(ECSManager& ecs, const Shader& light_shader)
{
    auto lb_mesh = std::make_unique<Mesh>();
    lb_mesh->loadOBJ("../assets/obj/light_bulb.obj");



    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    auto& render_comp = ecs.get_component<RenderComponent>(entity_);
    auto& transform_comp = ecs.get_component<TransformComponent>(entity_);
    auto& light_comp = ecs.get_component<LightComponent>(entity_);

    auto light = std::make_unique<bool>(true);

    render_comp->light = std::move(light);
    render_comp->shader = light_shader;
    render_comp->type_ = RenderType::RT_Mesh;
    render_comp->mesh_ = std::move(lb_mesh);

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
    light_comp->light_info.spot_angle_ = 20.0f;

    number_lights++;
    entity_++;

    return entity_;
}



size_t RenderSystem::createPointLight(ECSManager& ecs, Shader& light_shader)
{
    auto lb_mesh = std::make_unique<Mesh>();
    lb_mesh->loadOBJ("../assets/obj/light_bulb.obj");

  

    ecs.add_component<RenderComponent>(entity_);
    ecs.add_component<TransformComponent>(entity_);
    ecs.add_component<LightComponent>(entity_);

    auto& render_comp = ecs.get_component<RenderComponent>(entity_);
    auto& transform_comp = ecs.get_component<TransformComponent>(entity_);
    auto& light_comp = ecs.get_component<LightComponent>(entity_);
    auto light = std::make_unique<bool>(true);
    render_comp->light = std::move(light);
    render_comp->shader = light_shader;
    render_comp->type_ = RenderType::RT_Mesh;
    render_comp->mesh_ = std::move(lb_mesh);

    light_comp->shader = light_shader;
    light_comp->light_info.type_ = LightType::LT_Point;
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


size_t RenderSystem::createDirectionalLight(ECSManager& ecs, const Shader& light_shader)
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










