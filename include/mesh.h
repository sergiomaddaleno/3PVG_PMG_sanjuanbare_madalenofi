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
  
    Mesh();
    ~Mesh();

    bool loadOBJ(const std::string& file, const std::string& textureFolder = "");

    GLuint getTextureID(int shape_index);

private:
    GLuint shapeTextureID = 0;

};

#endif // MSL
