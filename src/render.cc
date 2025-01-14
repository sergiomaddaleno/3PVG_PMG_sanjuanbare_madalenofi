

#include "..\include\render.h"
#include "..\include\mesh.h"

#include <../deps/glm/glm.hpp>

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include "utils.h"
#include <memory> 
#include <iostream>

#define M_PI 3.14159265358979323846

Render::Render(){}

void Render::renderGeometry(RenderData& rd)
{
    GLuint vertex_buffer;
    GLint vpos_location, vcol_location;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rd.render_comp.triangle_->my_triangle[0]) * 3, rd.render_comp.triangle_->my_triangle, GL_STATIC_DRAW);

    vpos_location = glGetAttribLocation(rd.render_comp.shader.getProgram(), "vPos");
    vcol_location = glGetAttribLocation(rd.render_comp.shader.getProgram(), "vCol");

    rd.render_comp.shader.useProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rd.transf_comp.position.x, rd.transf_comp.position.y, rd.transf_comp.position.z));
    model = glm::rotate(model, rd.transf_comp.angle, glm::vec3(rd.transf_comp.rotation.x, rd.transf_comp.rotation.y, rd.transf_comp.rotation.z));
    model = glm::scale(model, glm::vec3(rd.transf_comp.scale.x, rd.transf_comp.scale.y, rd.transf_comp.scale.z));

    glm::mat4 view = rd.cam.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(rd.fov), (float)rd.w_width / rd.w_height, 0.1f, 100.0f);  
    glm::mat4 mvp = projection * view * model;

    GLint mvpLocation = glGetUniformLocation(rd.render_comp.shader.getProgram(), "MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vpos_location);

    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(vcol_location);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);

    glUseProgram(0);
}

void Render::renderMesh(RenderData &rd)
{
    for (auto& mesh : rd.render_comp.mesh_->meshes) {

        // ---
        glGenVertexArrays(1, &rd.render_comp.mesh_->VAO);
        glBindVertexArray(rd.render_comp.mesh_->VAO);

        glGenBuffers(1, &rd.render_comp.mesh_->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, rd.render_comp.mesh_->VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &rd.render_comp.mesh_->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rd.render_comp.mesh_->EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);
        
        // ---

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0); 
        // ---

        rd.render_comp.shader.useProgram();

        glCullFace(GL_BACK);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(rd.transf_comp.position.x, rd.transf_comp.position.y, rd.transf_comp.position.z));
        model = glm::rotate(model, rd.transf_comp.angle, glm::vec3(rd.transf_comp.rotation.x, rd.transf_comp.rotation.y, rd.transf_comp.rotation.z));
        model = glm::scale(model, glm::vec3(rd.transf_comp.scale.x, rd.transf_comp.scale.y, rd.transf_comp.scale.z));

        glm::mat4 view = rd.cam.getViewMatrix();
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

        glm::mat4 projection = glm::perspective(glm::radians(rd.fov), (float)rd.w_width / rd.w_height, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        // For shaders ---
        glUniformMatrix4fv(glGetUniformLocation(rd.render_comp.shader.getProgram(), "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(rd.render_comp.shader.getProgram(), "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(rd.render_comp.shader.getProgram(), "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(rd.render_comp.shader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Texture
        glActiveTexture(GL_TEXTURE0);
        rd.render_comp.texture = mesh.getTexture(0);
        rd.render_comp.texture.bind();
        glUniform1i(glGetUniformLocation(rd.render_comp.shader.getProgram(), "texture1"), 0);
        // ---

        glBindVertexArray(rd.render_comp.mesh_->VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); 

        glUseProgram(0);
        glDeleteVertexArrays(1, &rd.render_comp.mesh_->VAO);
        glDeleteBuffers(1, &rd.render_comp.mesh_->VBO);
        glDeleteBuffers(1, &rd.render_comp.mesh_->EBO);
    }
}

// ---

void Render::initMeshBuffers(ECSManager& ecs) {

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

void Render::renderSystem(ECSManager& ecs, RenderData& rd) {

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

void Render::renderLightSystem(ECSManager& ecs)
{
    auto li = ecs.getComponentList<LightComponent>().begin();
    auto liend = ecs.getComponentList<LightComponent>().end();

    for (; li != liend; ++li) {
        if (li->has_value()) {
            auto& light_comp = *li;

            for (auto& lights : light_comp->light_->lights) {

               
            }
        }
    }
}








