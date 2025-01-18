

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


void RenderSystem::initMesh(ECSManager& ecs) {

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


void RenderSystem::initLight(ECSManager& ecs)
{
}

void RenderSystem::renderLight(ECSManager& ecs, RenderData& rd) {
    // Obtener listas de componentes de luz y transformación
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

            // Enviar datos de luz al shader
            char name[60];
            sprintf(name, "u_light[%d].type_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_INT_1, &light_comp.light_.type_);

            sprintf(name, "u_light[%d].pos_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_3, glm::value_ptr(transform.position));

            sprintf(name, "u_light[%d].dir_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_3, glm::value_ptr(glm::normalize(light_comp.light_.dir_)));

            sprintf(name, "u_light[%d].diff_color_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_3, glm::value_ptr(light_comp.light_.diff_color_));

            sprintf(name, "u_light[%d].spec_color_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_3, glm::value_ptr(light_comp.light_.spec_color_));

            sprintf(name, "u_light[%d].ambient_color", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_3, glm::value_ptr(light_comp.light_.ambient_color));

            sprintf(name, "u_light[%d].constant_att_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_1, &light_comp.light_.constant_att_);

            sprintf(name, "u_light[%d].light_linear_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_1, &light_comp.light_.light_linear_);

            sprintf(name, "u_light[%d].light_quadratic_", counter);
            light_comp.shader.setUniformValue(light_comp.shader.getUniformLocation(name), Type::T_FLOAT_1, &light_comp.light_.light_quadratic_);

            counter++;

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

                        glm::mat4 model = glm::mat4(1.0f);
                        model = glm::translate(model, transform_comp->position);
                        model = glm::rotate(model, rd.transf_comp.angle, glm::vec3(rd.transf_comp.rotation.x, rd.transf_comp.rotation.y, rd.transf_comp.rotation.z));
                        model = glm::scale(model, transform_comp->scale);

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
                        // ---

                        //Fog
                        render_comp->shader.setUniformValue(render_comp->shader.getUniformLocation("u_fog"), Type::T_INT_1, &rd.b_fog_);

                        glBindVertexArray(mesh.VAO);
                        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
                        glBindVertexArray(0);
                    }
                }
            }

            glUseProgram(0);
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }

    glDisable(GL_BLEND);
}










