#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "mesh.h"
#include "texture.h"

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
    int materialIndex=0;

    bool obj = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str(), textureFolder.c_str());
    if (!obj) {
        printf("Failed to load OBJ file: ");
        printf("Warning: %s", warn.c_str());
        printf("Error: %s", err.c_str());
        return false;
    }

    for (const auto& shape : shapes) {
        Mesh curr_mesh;

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

            vertex.materialID = materialIndex;

            curr_mesh.vertices.push_back(vertex);
            curr_mesh.indices.push_back(static_cast<unsigned int>(curr_mesh.indices.size()));
        }

        if (textureFolder != "") {
            materialIndex = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];

            if (materialIndex >= 0 && materialIndex < materials.size()) {
                const auto& material = materials[materialIndex];

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

Texture Mesh::getTexture(int index)
{
    if (index >= 0 && index < textures.size()) {
        return textures[index];
    }

    return Texture();
}