

#include "..\include\render_system.h"
#include "..\include\mesh.h"
#include "utils.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"
#include "..\include\Components\light_component.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

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

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, transform_comp->position);
                model = glm::rotate(model, transform_comp->angle, transform_comp->rotation);
                model = glm::scale(model, transform_comp->scale);

                glm::mat4 view = rd.cam.getViewMatrix();
                glm::mat4 projection = glm::perspective(glm::radians(rd.fov), (float)rd.w_width / rd.w_height, 0.1f, 100.0f);
                glm::mat4 mvp = projection * view * model;

                // For shaders ---
                glUniformMatrix4fv(glGetUniformLocation(render_comp->shader.getProgram(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(glGetUniformLocation(render_comp->shader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(glGetUniformLocation(render_comp->shader.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(render_comp->shader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

                // For Texture
                glActiveTexture(GL_TEXTURE0);
                render_comp->texture = mesh.getTexture(0);
                render_comp->texture.bind();
                glUniform1i(glGetUniformLocation(render_comp->shader.getProgram(), "texture1"), 0);

                //For Fog
                GLuint fog_log = glGetUniformLocation(render_comp->shader.getProgram(), "u_fog");
                glUniform1i(fog_log, rd.b_fog_);

                //--
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

void RenderSystem::renderLight(ECSManager& ecs,RenderData& rd) {

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
           
            char name[60] = { '\0' };

            sprintf(name, "u_light[%d].type_\0", counter);
            glUniform1i(glGetUniformLocation(light_comp.shader.getProgram(), name), light_comp.light_.type_); // 0=Directional, 1=Spot, 2=Point
            
            sprintf(name, "u_light[%d].pos_\0", counter);
            glUniform3fv(glGetUniformLocation(light_comp.shader.getProgram(), name), 1, glm::value_ptr(transform.position));
            
            sprintf(name, "u_light[%d].dir_\0", counter);
            glUniform3fv(glGetUniformLocation(light_comp.shader.getProgram(), name), 1, glm::value_ptr(light_comp.light_.dir_));
           
            sprintf(name, "u_light[%d].diff_color_\0", counter);
            glUniform3fv(glGetUniformLocation(light_comp.shader.getProgram(), name), 1, glm::value_ptr(light_comp.light_.diff_color_));
            
            sprintf(name, "u_light[%d].spec_color_\0", counter);
            glUniform3fv(glGetUniformLocation(light_comp.shader.getProgram(), name), 1, glm::value_ptr(light_comp.light_.spec_color_));

            sprintf(name, "u_light[%d].light_color_\0", counter);
            glUniform3fv(glGetUniformLocation(light_comp.shader.getProgram(), name), 1, glm::value_ptr(light_comp.light_.light_color_));

            
            sprintf(name, "u_light[%d].ambient_color\0", counter);
            glUniform3fv(glGetUniformLocation(light_comp.shader.getProgram(), name), 1, glm::value_ptr(light_comp.light_.ambient_color));
            
            sprintf(name, "u_light[%d].light_linear_\0", counter);
            glUniform1f(glGetUniformLocation(light_comp.shader.getProgram(), name), light_comp.light_.light_linear_);
            
            sprintf(name, "u_light[%d].light_quadratic_\0", counter);
            glUniform1f(glGetUniformLocation(light_comp.shader.getProgram(), name), light_comp.light_.light_quadratic_);

            counter++;

            
            renderMesh(ecs, rd);
            glBlendFunc(GL_ONE, GL_ONE);
           
        }
    }
   
    glDisable(GL_BLEND);
}









