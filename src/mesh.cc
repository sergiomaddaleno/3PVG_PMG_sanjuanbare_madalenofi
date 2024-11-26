

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "../deps/tiny_obj_loader.h"
#include "../include/mesh.h"
#include "../deps/stb_image.h"
#include <filesystem>

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
    
    namespace fs = std::filesystem;

    std::string textureFolders = "C:/Users/pc000/Documents/GitHub/3PVG_PMG_sanjuanbare_madalenofi/assets/obj/";

    // Procesar y cargar texturas asociadas a los materiales
    for (const auto& material : materials) {  // Iterar sobre los objetos de tipo material
          // Asegúrate de que el nombre de la textura no esté vacío

            // Usamos std::filesystem::path para gestionar las rutas
            fs::path texturePath = fs::path(textureFolders) / material.diffuse_texname;

            // Depuración: Imprimir la ruta completa para asegurarse de que se construye correctamente
            std::cout << "Attempting to load texture from: " << texturePath << std::endl;

            // Verificar si el archivo existe
            if (!fs::exists(texturePath)) {
                std::cerr << "File not found: " << texturePath << std::endl;
                continue;  // Si no se encuentra el archivo, continuar con el siguiente
            }

            // Cargar la textura con stb_image
            int width, height, channels;
            unsigned char* data = stbi_load(texturePath.string().c_str(), &width, &height, &channels, 4);
            if (data) {
                // Cargar la textura en OpenGL
                
                glGenTextures(1, &textureID);
                glBindTexture(GL_TEXTURE_2D, textureID);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glBindTexture(GL_TEXTURE_2D, 0);  // Desvincula la textura

                stbi_image_free(data);

                std::cout << "Texture successfully loaded with ID: " << textureID << std::endl;  // Imprime el ID de textura
            }
            else {
                std::cerr << "Failed to load texture from: " << texturePath << std::endl;
                std::cerr << "Reason: " << stbi_failure_reason() << std::endl;  // Imprime el error
            }
        
    }

    return true;


}

