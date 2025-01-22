
#include "Render/skybox.h"
#include "Render/mesh.h"
#include "Render/Camera.h"

#include "Components/render_component.h"
#include "utils.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#include <memory> 
#include <iostream>

Skybox::Skybox() : skybox_angle(-90.0f), VAO(0), VBO(0), EBO(0) {}

Skybox::~Skybox()
{
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

void Skybox::render(RenderComponent rc, Camera cam) {
    glDepthMask(GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
    glm::mat4 projection = cam.getProjectionMatrix();
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(skybox_angle), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 model = rotation;

    float tex_id = 0.0f;
    rc.shader.useProgram();

    rc.texture.bind();
    rc.shader.setUniformValue(rc.shader.getUniformLocation("skybox"), Type::T_INT_1, &tex_id);

    unsigned int viewLoc = rc.shader.getUniformLocation("view");
    unsigned int projLoc = rc.shader.getUniformLocation("projection");
    unsigned int modelLoc = rc.shader.getUniformLocation("model");

    rc.shader.setUniformValue(viewLoc, Type::T_MAT_4x4, &view[0][0]);
    rc.shader.setUniformValue(projLoc, Type::T_MAT_4x4, &projection[0][0]);
    rc.shader.setUniformValue(modelLoc, Type::T_MAT_4x4, &model[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(gIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);
}

