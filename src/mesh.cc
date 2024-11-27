

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "../deps/tiny_obj_loader.h"
#include "../include/mesh.h"
#include "..\include\render.h"
#include "../deps/stb_image.h"

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

    bool obj = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,file.c_str(), textureFolder.c_str());

    if (!warn.empty()) {
        printf("Warning: %s\n", warn.c_str());
    }
    if (!err.empty()) {
        printf("Error: %s\n", err.c_str());
    }

    if (!obj) {
        printf("Failed to load OBJ: %s\n", file.c_str());
        return false;
    }

    for (const auto& shape : shapes) {
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
                vertex.tex_coord[1] = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }

    if (materials.empty()) {
        printf("No materials found in the OBJ. Make sure the .mtl file is present.\n");
        return true;
    }
    

    const auto& material = materials[0];
    if (!material.diffuse_texname.empty()) {
        std::string texturePath = textureFolder + "/" + material.diffuse_texname;

        int width, height, channels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
        if (!data) {
            printf("Failed to load texture: %s\n", texturePath.c_str());
            return false;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
    }
    else {
        printf("Material has no diffuse texture.\n");
    }

    return true;
}

GLuint Mesh::GetTextureID()
{
    return textureID;
}

