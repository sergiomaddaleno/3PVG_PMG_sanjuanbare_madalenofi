/**
 * @file cube.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __CUBE__
#define __CUBE__ 1

#include <memory>
#include <vector>
#include "Core/texture.h"
#include "Geometry/wave.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Cube
  * @brief Represents a 3D cube.
  */
class Cube {
public:
    Cube();
    ~Cube();

    void init8v(const float cube_size = 1.0f, bool is_centered = true);
    void init24v(const float cube_size = 1.0f);

    void bind();
    void draw();
    void setTexture(std::string file_path);

    std::vector<float> GetVertexCube8v();
    std::vector<float> GetVertexCube24v();

    nlohmann::json serialize() const;

    //---
    float size_;
    bool is_centered_;
    bool cube_8v_;
    bool cube_24v_;

    GerstnerWave wave_;

    Texture texture_;
    bool use_texture_;
    std::string texture_path_;

private:
    std::vector<int> order_;
    std::vector<float> data_;

    glm::vec3 vertex_cube8v_[8];
    glm::vec3 vertex_cube24v_[24];

    GLuint VAO, VBO, EBO;
};

#endif //__CUBE__