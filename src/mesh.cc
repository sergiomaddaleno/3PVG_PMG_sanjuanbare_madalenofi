#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "../deps/tiny_obj_loader.h"
#include "../include/mesh.h"
#include "..\include\render.h"
#include "../deps/stb_image.h"

std::vector<Mesh> meshes; // Vector para almacenar las mallas

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

bool Mesh::LoadOBj(const std::string& file, const std::string& textureFolder)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool obj = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str(), textureFolder.c_str());

    for (const auto& shape : shapes) {
        Mesh currentMesh; // Crear una malla para cada forma

        int materialIndex = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];

        if (materialIndex >= 0 && materialIndex < materials.size()) {
            const auto& material = materials[materialIndex];

            if (!material.diffuse_texname.empty()) {
                std::string texturePath = textureFolder + "/" + material.diffuse_texname;

                int width, height, channels;
                unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);

                glGenTextures(1, &currentMesh.shapeTextureID);
                glBindTexture(GL_TEXTURE_2D, currentMesh.shapeTextureID);

                currentMesh.shapeTextureIDs.push_back(currentMesh.shapeTextureID);

                GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glBindTexture(GL_TEXTURE_2D, 0);
                stbi_image_free(data);
            }
        }

        for (const auto& index : shape.mesh.indices) {
            Vertex vertex = {};

            vertex.pos[0] = attrib.vertices[3 * index.vertex_index + 0];
            vertex.pos[1] = attrib.vertices[3 * index.vertex_index + 1];
            vertex.pos[2] = attrib.vertices[3 * index.vertex_index + 2];

            if (!attrib.normals.empty()) {
                vertex.normal[0] = attrib.normals[3 * index.normal_index + 0];
                vertex.normal[1] = attrib.normals[3 * index.normal_index + 1];
                vertex.normal[2] = attrib.normals[3 * index.normal_index + 2];
            }

            if (!attrib.texcoords.empty()) {
                vertex.tex_coord[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.tex_coord[1] = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]; //uvs invertidos
            }

            vertex.materialID = materialIndex;

            currentMesh.vertices.push_back(vertex);
            currentMesh.indices.push_back(static_cast<unsigned int>(currentMesh.indices.size()));
        }

        meshes.push_back(currentMesh);
    }

    return true;
}

GLuint Mesh::GetTextureID(int shape_index)
{
    if (shape_index >= 0 && shape_index < shapeTextureIDs.size()) {
        return shapeTextureIDs[shape_index];
    }
    return 0;
}
