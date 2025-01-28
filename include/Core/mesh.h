#ifndef MSL
#define MSL

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <vector>
#include <../deps/glm/glm.hpp>

#include "texture.h"
#include "Render/camera.h"

#include "../deps/nlohmann/json.hpp"

struct RenderComponent;
struct TransformComponent;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 tex_coord;
};

class Mesh {

public:
    
    std::vector<Vertex> vertices;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;

    std::vector<unsigned int> indices;
    int materialID;

    GLuint VAO = 0, VBO = 0, EBO = 0;

    //---

    bool loadOBJ(const std::string& file, const std::string& textureFolder = "");

    void render(RenderComponent rc, TransformComponent tc, Camera cam = Camera());

    Texture getTexture(int shape_index);

    //Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        for (const auto& mesh : meshes) {
            json_data["meshes"].push_back(mesh.serialize());
        }

        for (const auto& texture : textures) {
            json_data["textures"].push_back(texture.serialize());
        }

        json_data["materialID"] = materialID;
        json_data["VAO"] = VAO;
        json_data["VBO"] = VBO;
        json_data["EBO"] = EBO;

        return json_data;
    }

    //--

    Mesh() : VAO(0), VBO(0), EBO(0), materialID(0){};

    ~Mesh() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    };   
};

#endif // MSL
