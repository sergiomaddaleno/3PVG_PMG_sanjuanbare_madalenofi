#ifndef __MESH__
#define __MESH__

#include <string>
#include <iostream>
#include <vector>
#include <../deps/glm/glm.hpp>
#include "../deps/nlohmann/json.hpp"

#include "texture.h"
#include "Render/camera.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct RenderComponent;
struct TransformComponent;

class Mesh {

public:
    Mesh();
    ~Mesh();
    
    bool loadOBJ(const std::string& file, const std::string& textureFolder = "");

    void bind();
    void draw();

    Texture getTexture(int shape_index);

    // ---

    struct Vertex {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 tex_coord;
    };

    std::vector<Vertex> vertices;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;

    std::vector<unsigned int> indices;
    int materialID;

    bool use_texture_;
    glm::vec3 color_;

    std::string mesh_path;
    std::string texture_folder;

    GLuint VAO, VBO, EBO;

    //Save data
    nlohmann::json serialize() const;
};

#endif // __MESH__
