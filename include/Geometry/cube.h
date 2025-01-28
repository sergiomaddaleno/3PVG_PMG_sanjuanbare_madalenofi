

#ifndef __CUBE_H__
#define __CUBE_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/texture.h"
#include <memory>
#include <vector>

class Cube {
 public:
     Cube();
    ~Cube();
 
    void init8v(const float cube_size = 1.0f);
    void init24v(const float cube_size = 1.0f);

    GLuint VAO = 0, VBO = 0, EBO = 0;

    std::vector<int> order_;
    std::vector<float> data_;

    Texture tex;
    float size_ = 0.0f;
    bool cube_8v_= false;
    bool cube_24v_ = false;

    //Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        json_data["Texture"].push_back(tex.serialize());
        json_data["size"] = size_;
        json_data["cube_8v"] = cube_8v_;
        json_data["cube_24v"] = cube_24v_;

        return json_data;
    }
};


#endif //__CUBE_H__