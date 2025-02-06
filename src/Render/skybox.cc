
#include "Render/skybox.h"
#include "Core/mesh.h"
#include "Render/Camera.h"

#include "Components/render_component.h"
#include "utils.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#include <memory> 
#include <iostream>

Skybox::Skybox() : skybox_angle_(-90.0f), VAO(0), VBO(0), EBO(0),
dds_file_("../assets/textures/skybox/sky.dds"), 
vertex_file_("../assets/shaders/skybox.vs"), 
fragment_file_("../assets/shaders/skybox.fs")
{}

Skybox::~Skybox()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Skybox::init() {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gMesh), gMesh, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gIndices), gIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::render(Camera cam) {
    glDepthMask(GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_LEQUAL);

    glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
    glm::mat4 projection = cam.getProjectionMatrix();
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(skybox_angle_), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 model = rotation;

    int tex_id = 1;
    shader_.useProgram();

    texture_.bind();
    shader_.setUniformValue(shader_.getUniformLocation("skybox"), ValueType::T_INT_1, &tex_id);
    shader_.setUniformValue(shader_.getUniformLocation("view"), ValueType::T_MAT_4x4, &view[0][0]);
    shader_.setUniformValue(shader_.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection[0][0]);
    shader_.setUniformValue(shader_.getUniformLocation("model"), ValueType::T_MAT_4x4, &model[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(gIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);
}

void Skybox::loadSkyBox(const char* dds_file, const char* vertex_file, const char* fragment_file)
{
    dds_file_ = dds_file;
    vertex_file_ = vertex_file;
    fragment_file_ = fragment_file;

    char* loc_file = ReadFile(dds_file);
    char* loc_vert = ReadFile(vertex_file);
    char* loc_frag = ReadFile(fragment_file);

    if (!texture_.loadSkyboxTexture(loc_file)) {
        printf("Error in loading skybox\n");
    }

    shader_.compile(loc_vert, loc_frag);

    free(loc_file);
    free(loc_vert);
    free(loc_frag);
}

nlohmann::json Skybox::serialize() const
{
    nlohmann::json json_data;

    json_data["Skybox_file_path"] = dds_file_;
    json_data["Vertex_source"] = vertex_file_;
    json_data["Fragment_source"] = fragment_file_;
    json_data["angle"] = skybox_angle_;

    return json_data;
}

