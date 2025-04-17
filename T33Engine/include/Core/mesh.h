/**
 * @file mesh.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __MESH__
#define __MESH__

#include <string>
#include <iostream>
#include <vector>

#include "texture.h"
#include "Core/camera.h"
#include "Core/shader.h"
#include "Geometry/wave.h"

#include <../deps/glm/glm.hpp>
#include "../deps/nlohmann/json.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct RenderComponent;
struct TransformComponent;

/**
 * @class Mesh
 * @brief Represents a 3D mesh with vertices, textures, and rendering capabilities.
 */
class Mesh {

public:
    Mesh();
    ~Mesh();

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };
    
    bool loadOBJ(const std::string& file, const std::string& textureFolder = "");

    void bind();
    void draw();

    std::vector<float> getVertex();
    Texture getTexture(int shape_index);

    std::vector<Vertex> Getvertices();
    GLuint getVAO();
    GLuint getVBO();
    GLuint getEBO();

    nlohmann::json serialize() const;

    GerstnerWave wave_;
    bool use_texture_;

    std::vector<Mesh> meshes;
    std::string mesh_path;
    std::string texture_folder;

private:
    std::vector<Vertex> vertices;
    std::vector<Texture> textures;

    std::vector<unsigned int> indices;
    int materialID;

    GLuint VAO, VBO, EBO;
};

#endif // !__MESH__
