

#ifndef __CUBE_H__
#define __CUBE_H__ 1

#include <memory>
#include <vector>
#include "Core/texture.h"
#include "Geometry/wave.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Cube {
public:
    Cube();
    ~Cube();

    void init8v(const float cube_size = 1.0f, bool is_centered = true);
    void init24v(const float cube_size = 1.0f);

    void bind();
    void draw();
    void setTexture(std::string file_path);

    glm::vec3* GetVertexCube8v();
    glm::vec3* GetVertexCube24v();

    nlohmann::json serialize() const;

    //---

    GLuint VAO, VBO, EBO;

    float size_;
    bool is_centered_;
    bool cube_8v_;
    bool cube_24v_;

    GerstnerWave wave_;

    Texture texture_;
    bool use_texture_;
    glm::vec3 color_;
    std::string texture_path_;

private:
    std::vector<int> order_;
    std::vector<float> data_;

    glm::vec3 vertex_cube8v_[8];
    glm::vec3 vertex_cube24v_[24];
};


#endif //__CUBE_H__