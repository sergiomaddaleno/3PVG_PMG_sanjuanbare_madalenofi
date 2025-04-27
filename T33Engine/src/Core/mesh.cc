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

Mesh::Mesh() : VAO(0), VBO(0), EBO(0), materialID(0),
use_texture_(true)
{}

Mesh::~Mesh()
{
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
};

bool Mesh::loadOBJ(const std::string& file,
    const std::string& textureFolder)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    mesh_path = file;
    texture_folder = textureFolder;

    bool obj = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str(), textureFolder.c_str());
    if (!obj) {
        AddConsoleMessage("Error: Failed to load OBJ file");
        AddConsoleMessage("Warning: " + warn);
        AddConsoleMessage("Error: " + err);
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
            else {
                vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
            }

            if (!attrib.texcoords.empty()) {
                vertex.tex_coord.x = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.tex_coord.y = attrib.texcoords[2 * index.texcoord_index + 1];
            }
            else {
                vertex.tex_coord = glm::vec2(0.0f, 0.0f);
            }

            curr_mesh.vertices.push_back(vertex);
            curr_mesh.indices.push_back(static_cast<unsigned int>(curr_mesh.indices.size()));
        }

        //For textures ---
        if (!textureFolder.empty()) {
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
                else {
                    Texture tex;
                    tex.createColorTexture(glm::vec3(
                        material.diffuse[0],
                        material.diffuse[1],
                        material.diffuse[2]
                    ));
                    curr_mesh.textures.push_back(tex);
                }
            }
        }
        meshes.push_back(curr_mesh);
    }

    return true;
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

void Mesh::draw() {
    
    for (auto& mesh : meshes) {
        
        if (!mesh.textures.empty() && use_texture_) {
            mesh.textures[0].active(TextureID::TI_Diffuse);
            mesh.textures[0].bind2D();
        }
         
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

std::vector<float> Mesh::getVertex()
{
    std::vector<float> vertex_float;
    for (const auto& vertex : vertices) {
        vertex_float.push_back(vertex.pos.x);
        vertex_float.push_back(vertex.pos.y);
        vertex_float.push_back(vertex.pos.z);
    }
    return vertex_float;
}

Texture Mesh::getTexture(int index)
{
    if (index >= 0 && index < textures.size()) {
        return textures[index];
    }

    return Texture();
}

std::vector<Mesh::Vertex> Mesh::Getvertices()
{
    return vertices;
}

GLuint Mesh::getVAO() {
    return VAO;
}

GLuint Mesh::getVBO()
{
    return VBO;
}

GLuint Mesh::getEBO()
{
    return EBO;
}

nlohmann::json Mesh::serialize() const
{
    nlohmann::json json_data;

    json_data["Mesh_path"] = mesh_path;
    json_data["Texture_folder"] = texture_folder;

    return json_data;
}
