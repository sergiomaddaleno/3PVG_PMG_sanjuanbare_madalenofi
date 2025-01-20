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
        model = glm::translate(model, rd.transf_comp.position);
        model = model * glm::mat4_cast(rd.transf_comp.rotation);
        model = glm::scale(model, rd.transf_comp.scale);

        glm::mat4 view = rd.cam.getViewMatrix();
        glm::mat4 projection = rd.cam.getProjectionMatrix();
        glm::mat4 mvp = projection * view * model;

        // For shaders ---
        rd.render_comp.shader.setUniformValue(rd.render_comp.shader.getUniformLocation("mvp"), Type::T_MAT_4x4, glm::value_ptr(mvp));
        rd.render_comp.shader.setUniformValue(rd.render_comp.shader.getUniformLocation("model"), Type::T_MAT_4x4, glm::value_ptr(model));
        rd.render_comp.shader.setUniformValue(rd.render_comp.shader.getUniformLocation("view"), Type::T_MAT_4x4, glm::value_ptr(view));
        rd.render_comp.shader.setUniformValue(rd.render_comp.shader.getUniformLocation("projection"), Type::T_MAT_4x4, glm::value_ptr(projection));

        // Texture
        float tex_id = 0.0f;
        rd.render_comp.texture = mesh.getTexture(0);
        rd.render_comp.texture.bind();
        rd.render_comp.shader.setUniformValue(rd.render_comp.shader.getUniformLocation("texture1"), Type::T_INT_1, &tex_id);
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