
#include "skybox.h"
#include "..\include\render.h"
#include "..\include\mesh.h"

#include <../deps/glm/glm.hpp>

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include "utils.h"
#include <memory> 
#include <iostream>

Skybox::Skybox() : skybox_angle(-90.0f), VAO(0), VBO(0), EBO(0) {}

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

void Skybox::render(RenderData& rd) {
    glDepthMask(GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(glm::radians(rd.fov), (float)rd.w_width / rd.w_height, 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(glm::mat3(rd.cam.getViewMatrix())); // Only rotation, no translation

    // Fix skybox rotation
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(skybox_angle), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 model = rotation;

    rd.render_comp.skybox_shader.useProgram();
    rd.render_comp.skybox_tex.bind();
    glUniform1i(glGetUniformLocation(rd.render_comp.skybox_shader.getProgram(), "skybox"), 0);
    glActiveTexture(GL_TEXTURE0);

    unsigned int viewLoc = glGetUniformLocation(rd.render_comp.skybox_shader.getProgram(), "view");
    unsigned int projLoc = glGetUniformLocation(rd.render_comp.skybox_shader.getProgram(), "projection");
    unsigned int modelLoc = glGetUniformLocation(rd.render_comp.skybox_shader.getProgram(), "model");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(gIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glDepthMask(GL_TRUE);

    assert(glGetError() == GL_NO_ERROR);
}
