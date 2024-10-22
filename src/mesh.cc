
#define TINYOBJLOADER_IMPLEMENTATION
#include "../deps/tiny_obj_loader.h"
#include "..\include\mesh.h"

bool Mesh::LoadOBj(const std::string& file)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

	bool obj = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.c_str());

    if (!obj) {
        printf("Fail to charge OBJ");
        return false;
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex = {};
            vertex.pos[0] = {
                attrib.vertices[3 * index.vertex_index + 0]
            }; 
            vertex.pos[1] = {
                attrib.vertices[3 * index.vertex_index + 1]
            };

            vertex.pos[2] = {
                attrib.vertices[3 * index.vertex_index + 2]
            };

            if (!attrib.normals.empty()) {
                vertex.normal[0] = {
                    attrib.normals[3 * index.normal_index + 0]
                };

                vertex.normal[1] = {
                    attrib.normals[3 * index.normal_index + 1]
                };

                vertex.normal[2] = {
                    attrib.normals[3 * index.normal_index + 2]
                };
              
            }

            if (!attrib.texcoords.empty()) {
                vertex.tex_coord[0] = {
                    attrib.texcoords[2 * index.texcoord_index + 0]
                };
               vertex.tex_coord[1] = {
                    attrib.texcoords[2 * index.texcoord_index + 1]
                };

            }
            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

    return true;

}
