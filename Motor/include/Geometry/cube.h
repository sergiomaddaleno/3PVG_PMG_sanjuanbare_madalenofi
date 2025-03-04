

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

    nlohmann::json serialize() const;

    //---

    GLuint VAO, VBO, EBO;

    float size_;
    bool cube_8v_;
    bool cube_24v_;

    GerstnerWave wave_;

    Texture texture_;
    bool use_texture_;
    glm::vec3 color_;
    std::string texture_path_;

    std::vector<int> order_;
    std::vector<float> data_;
};


#endif //__CUBE_H__