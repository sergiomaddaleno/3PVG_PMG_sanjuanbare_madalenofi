#ifndef MSL
#define MSL

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <vector>
#include <../deps/glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 tex_coord;

    int materialID;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<GLuint> shapeTextureIDs;
    std::vector<Mesh> meshes;
    GLuint VAO = 0, VBO = 0, EBO = 0;
  
    Mesh() {
        VAO = 0;
        VBO = 0;
        EBO = 0;
    };

    ~Mesh() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    };

    bool loadOBJ(const std::string& file, const std::string& textureFolder = "");

    GLuint getTextureID(int shape_index);

private:
    GLuint shapeTextureID = 0;

};

#endif // MSL
