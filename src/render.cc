
#include "..\include\render.h"
#include "..\include\mesh.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <memory> 
#include <iostream>

#define M_PI 3.14159265358979323846

Render::Render() {}

std::optional<Render> Render::make()
{  
    glewInit();
   
    return std::optional<Render>();
}

GLuint Render::getProgram()
{
    return program_;
}

ShaderBuffers Render::initShader(const char* vertex_shader_path, const char* fragment_shader_path,
    ShaderBuffers &shaders) {

    FILE* file = fopen(vertex_shader_path, "rb");
    long size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        shaders.vs = (char*)malloc(size + 1);
        if (shaders.vs) {
            fread(shaders.vs, sizeof(char), size, file);
            shaders.vs[size] = '\0';

            //printf("%s\n", shaders.vs);
        }
        fclose(file);
    }
    else {
        printf("Error in loading Vertex Shader\n");
    }

    file = fopen(fragment_shader_path, "rb");
    size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        shaders.fs = (char*)malloc(size + 1);
        if (shaders.fs) {
            fread(shaders.fs, sizeof(char), size, file);
            shaders.fs[size] = '\0';

            //printf("%s\n", shaders.fs);
        }
        fclose(file);
    }
    else {
        printf("Error in loading Fragment Shader\n");
    }

    return shaders;
}

void Render::setupShader(ShaderBuffers &shaders) {

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &shaders.vs, nullptr);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &shaders.fs, nullptr);
    glCompileShader(fragmentShader);

    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void Render::renderGeometry(RenderComponent& rc, TransformComponent& tr)
{
    GLuint vertex_buffer;
    GLint vpos_location, vcol_location;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rc.triangle_->my_triangle[0]) * 3, rc.triangle_->my_triangle, GL_STATIC_DRAW);

    vpos_location = glGetAttribLocation(program_, "vPos");
    vcol_location = glGetAttribLocation(program_, "vCol");

    glUseProgram(program_);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(tr.position.x, tr.position.y, tr.position.z));
    model = glm::rotate(model, tr.angle, glm::vec3(tr.rotation.x, tr.rotation.y, tr.rotation.z));
    model = glm::scale(model, glm::vec3(tr.scale.x, tr.scale.y, tr.scale.z));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); 

    glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)800 / 600, 0.1f, 100.0f);  
    glm::mat4 mvp = projection * view * model;

    GLint mvpLocation = glGetUniformLocation(program_, "MVP");
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

void Render::renderMesh(RenderComponent& rc, TransformComponent& tc)
{
    for (auto& mesh : rc.mesh_->meshes) {

        // ---
        glGenVertexArrays(1, &rc.VAO);
        glBindVertexArray(rc.VAO);

        glGenBuffers(1, &rc.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, rc.VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &rc.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rc.EBO);
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

        glUseProgram(program_);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(tc.position.x, tc.position.y, tc.position.z));
        model = glm::rotate(model, tc.angle, glm::vec3(tc.rotation.x, tc.rotation.y, tc.rotation.z));
        model = glm::scale(model, glm::vec3(tc.scale.x, tc.scale.y, tc.scale.z));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

        glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)800 / 600, 0.1f, 100.0f);
        glm::mat4 mvp = projection * view * model;

        // For shaders ---
        glUniformMatrix4fv(glGetUniformLocation(program_, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Texture
        glActiveTexture(GL_TEXTURE0);
        rc.textureID = mesh.getTextureID(0);
        if (rc.textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, rc.textureID);
            glUniform1i(glGetUniformLocation(program_, "texture1"), 0);
        }
        // ---

        glBindVertexArray(rc.VAO);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); 

        glUseProgram(0);
        glDeleteVertexArrays(1, &rc.VAO);
        glDeleteBuffers(1, &rc.VBO);
        glDeleteBuffers(1, &rc.EBO);
    }
}

void Render::renderSystem(std::vector<std::optional<RenderComponent>>& rc, std::vector<std::optional<TransformComponent>>& tc)
{
    auto ri = rc.begin();
    auto ti = tc.begin();
    auto riend = rc.end();
    auto tiend = tc.end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            for (auto& mesh : render_comp->mesh_->meshes) {
                
                // ---
                glGenVertexArrays(1, &render_comp->VAO);
                glBindVertexArray(render_comp->VAO);

                glGenBuffers(1, &render_comp->VBO);
                glBindBuffer(GL_ARRAY_BUFFER, render_comp->VBO);
                glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

                glGenBuffers(1, &render_comp->EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_comp->EBO);
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

                glUseProgram(program_);

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(transform_comp->position.x, transform_comp->position.y, transform_comp->position.z));
                model = glm::rotate(model, transform_comp->angle, glm::vec3(transform_comp->rotation.x, transform_comp->rotation.y, transform_comp->rotation.z));
                model = glm::scale(model, glm::vec3(transform_comp->scale.x, transform_comp->scale.y, transform_comp->scale.z));

                glm::mat4 view = glm::mat4(1.0f);
                view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));

                glm::mat4 projection = glm::perspective(glm::radians(55.0f), (float)800 / 600, 0.1f, 100.0f);
                glm::mat4 mvp = projection * view * model;

                // For shaders ---
                glUniformMatrix4fv(glGetUniformLocation(program_, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
                glUniformMatrix4fv(glGetUniformLocation(program_, "model"), 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(glGetUniformLocation(program_, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(program_, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

                // Texture
                glActiveTexture(GL_TEXTURE0);
                render_comp->textureID = mesh.getTextureID(0);
                if (render_comp->textureID != 0) {
                    glBindTexture(GL_TEXTURE_2D, render_comp->textureID);
                    glUniform1i(glGetUniformLocation(program_, "texture1"), 0);
                }
                // ---

                glBindVertexArray(render_comp->VAO);
                glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

                glUseProgram(0);
                glDeleteVertexArrays(1, &render_comp->VAO);
                glDeleteBuffers(1, &render_comp->VBO);
                glDeleteBuffers(1, &render_comp->EBO);
            }
        }
    }
}







