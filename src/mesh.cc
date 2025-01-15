#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "mesh.h"
#include "texture.h"
#include "utils.h"

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#pragma warning(push)
#pragma warning(disable:26495)
#include "../deps/tiny_obj_loader.h"
#pragma warning(push)
#include "../deps/stb_image.h"


bool Mesh::loadOBJ(const std::string& file, const std::string& textureFolder)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool obj = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str(), textureFolder.c_str());
    if (!obj) {
        printf("Failed to load OBJ file: ");
        printf("Warning: %s", warn.c_str());
        printf("Error: %s", err.c_str());
        return false;
    }

    for (const auto& shape : shapes) {
        Mesh curr_mesh;

        //For mesh
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex = {};

            vertex.pos.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.pos.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.pos.z = attrib.vertices[3 * index.vertex_index + 2];

            if (!attrib.normals.empty()) {
                vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
                vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
                vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
            }

            if (!attrib.texcoords.empty()) {
                vertex.tex_coord.x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.tex_coord.y = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            curr_mesh.vertices.push_back(vertex);
            curr_mesh.indices.push_back(static_cast<unsigned int>(curr_mesh.indices.size()));
        }

        //For textures ---
        if (textureFolder != "") {
            materialID = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];

            if (materialID >= 0 && materialID < materials.size()) {
                const auto& material = materials[materialID];

                if (!material.diffuse_texname.empty()) {
                    std::string texturePath = textureFolder + "/" + material.diffuse_texname;

                    Texture tex;
                    if (tex.loadTexture(texturePath)) {
                        curr_mesh.textures.push_back(tex);
                    }
                }
            }
        }
        meshes.push_back(curr_mesh);
    }

    return true;
}

void Mesh::render(RenderData& rd)
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


Texture Mesh::getTexture(int index)
{
    if (index >= 0 && index < textures.size()) {
        return textures[index];
    }

    return Texture();
}