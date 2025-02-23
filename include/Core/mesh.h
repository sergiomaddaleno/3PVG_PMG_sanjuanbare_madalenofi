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

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

class Mesh {

public:
    
    bool loadOBJ(const std::string& file, const std::string& textureFolder = "");

    void render(RenderComponent rc, TransformComponent tc, Camera cam = Camera());

    void bind();
    void draw();

    Texture getTexture(int shape_index);

    // ---

    std::vector<Vertex> vertices;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;

    std::vector<unsigned int> indices;
    int materialID;

    std::string mesh_path;
    std::string texture_folder;

    GLuint VAO = 0, VBO = 0, EBO = 0;

    //Save data
    nlohmann::json serialize() const;

    //--

    Mesh() : VAO(0), VBO(0), EBO(0), materialID(0){};

    ~Mesh() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    };   
};

#endif // __MESH__
