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

Texture Mesh::getTexture(int index)
{
    if (index >= 0 && index < textures.size()) {
        return textures[index];
    }

    return Texture();
}