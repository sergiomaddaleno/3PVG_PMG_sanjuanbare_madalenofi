

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "../deps/tiny_obj_loader.h"
#include "../include/mesh.h"
#include "../deps/stb_image.h"

bool Mesh::LoadOBj(const std::string& file, const std::string& textureFolder)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Cargar el archivo OBJ y MTL
    bool obj = tinyobj::LoadObj(
        &attrib, &shapes, &materials, &warn, &err,
        file.c_str(), textureFolder.c_str()
    );

    // Imprimir advertencias y errores si existen
    if (!warn.empty()) {
        printf("Warning: %s\n", warn.c_str());
    }
    if (!err.empty()) {
        printf("Error: %s\n", err.c_str());
    }

    // Verificar si hubo un error al cargar el OBJ
    if (!obj) {
        printf("Failed to load OBJ: %s\n", file.c_str());
        return false;
    }

    // Cargar vértices, normales y coordenadas de textura
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex = {};

            // Cargar posiciones del vértice
            vertex.pos[0] = attrib.vertices[3 * index.vertex_index + 0];
            vertex.pos[1] = attrib.vertices[3 * index.vertex_index + 1];
            vertex.pos[2] = attrib.vertices[3 * index.vertex_index + 2];

            // Cargar normales (si existen)
            if (!attrib.normals.empty()) {
                vertex.normal[0] = attrib.normals[3 * index.normal_index + 0];
                vertex.normal[1] = attrib.normals[3 * index.normal_index + 1];
                vertex.normal[2] = attrib.normals[3 * index.normal_index + 2];
            }

            // Cargar coordenadas de textura (si existen)
            if (!attrib.texcoords.empty()) {
                vertex.tex_coord[0] = attrib.texcoords[2 * index.texcoord_index + 0];
                vertex.tex_coord[1] = attrib.texcoords[2 * index.texcoord_index + 1];
            }

            // Agregar vértices e índices
            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(indices.size()));
        }
    }

    // Si los materiales están vacíos, significa que no se cargaron correctamente
    if (materials.empty()) {
        printf("No materials found in the OBJ. Make sure the .mtl file is present.\n");
    }

    // Procesar y cargar texturas asociadas a los materiales
    for (const auto& material : materials) {
        if (!material.diffuse_texname.empty()) {
            std::string texturePath = textureFolder + "/" + material.diffuse_texname;

            // Verificar si la ruta de la textura es válida
            printf("Loading texture: %s\n", texturePath.c_str());

            int width, height, channels;
            unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);

            if (data) {
                printf("Texture loaded: %s (Width: %d, Height: %d)\n", texturePath.c_str(), width, height);

                // Aquí deberías cargar la textura en tu API gráfica (OpenGL, Vulkan, etc.)
                stbi_image_free(data);
            }
            else {
                // Falló la carga de la textura
                printf("Failed to load texture: %s\n", texturePath.c_str());
            }
        }
    }

    return true;
}

