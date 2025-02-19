#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "Core/mesh.h"
#include "Core/texture.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"
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

    mesh_path = file;
    texture_folder = textureFolder;
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

void Mesh::render(RenderComponent rc, TransformComponent tc, Camera cam)
{
    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (auto& mesh : rc.mesh_->meshes) {

        // ---
        glGenVertexArrays(1, &rc.mesh_->VAO);
        glBindVertexArray(rc.mesh_->VAO);

        glGenBuffers(1, &rc.mesh_->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, rc.mesh_->VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &rc.mesh_->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rc.mesh_->EBO);
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

        rc.shader.useProgram();

        //glCullFace(GL_BACK);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, tc.position);
        model = model * glm::mat4_cast(tc.rotation);
        model = glm::scale(model, tc.scale);

        glm::mat4 view = cam.getViewMatrix();
        glm::mat4 projection = cam.getProjectionMatrix();
        glm::mat4 mvp = projection * view * model;

        // For shaders ---
        rc.shader.setUniformValue(rc.shader.getUniformLocation("mvp"), ValueType::T_MAT_4x4, glm::value_ptr(mvp));
        rc.shader.setUniformValue(rc.shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
        rc.shader.setUniformValue(rc.shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
        rc.shader.setUniformValue(rc.shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

        // Texture
        float tex_id = 0.0f;
        rc.texture = mesh.getTexture(0);
        rc.texture.bind();
        rc.shader.setUniformValue(rc.shader.getUniformLocation("texture1"), ValueType::T_INT_1, &tex_id);
        // ---

        // Fog
        rc.shader.setUniformValue(rc.shader.getUniformLocation("u_fog"), ValueType::T_INT_1, &g_b_fog_);


        glBindVertexArray(rc.mesh_->VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glUseProgram(0);
        glDeleteVertexArrays(1, &rc.mesh_->VAO);
        glDeleteBuffers(1, &rc.mesh_->VBO);
        glDeleteBuffers(1, &rc.mesh_->EBO);
    }
    */
}

void Mesh::bind()
{
    for (auto& mesh : meshes) {

        glGenVertexArrays(1, &mesh.VAO);
        glBindVertexArray(mesh.VAO);

        glGenBuffers(1, &mesh.VBO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &mesh.EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

        //---

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

void Mesh::renderElements(bool use_texture) {
    
    for (auto& mesh : meshes) {
        
        if (!use_texture || !mesh.textures.empty()) {
            mesh.textures[0].bind();
        }
        
         glBindVertexArray(mesh.VAO);
         glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
         glBindVertexArray(0);
    }
}

Texture Mesh::getTexture(int index)
{
    if (index >= 0 && index < textures.size()) {
        return textures[index];
    }

    return Texture();
}

nlohmann::json Mesh::serialize() const
{
    nlohmann::json json_data;

    for (const auto& mesh : meshes) {
        json_data["Meshes"].push_back(mesh.serialize());
    }

    for (const auto& texture : textures) {
        json_data["Textures"].push_back(texture.serialize());
    }

    json_data["MaterialID"] = materialID;
    json_data["Mesh_path"] = mesh_path;
    json_data["Texture_folder"] = texture_folder;

    return json_data;
}
