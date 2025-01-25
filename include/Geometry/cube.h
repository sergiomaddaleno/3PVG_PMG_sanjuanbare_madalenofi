

#ifndef __CUBE_H__
#define __CUBE_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Render/texture.h"
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

    //Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        json_data["Texture"].push_back(tex.serialize());
        json_data["VAO"] = VAO;
        json_data["VBO"] = VBO;
        json_data["EBO"] = EBO;

        return json_data;
    }

    nlohmann::json deSerialize(nlohmann::json& j) {
        tex.deSerialize(j["Texture"][0]);
        VAO = j["VAO"].get<GLuint>();
        VBO = j["VBO"].get<GLuint>();
        EBO = j["EBO"].get<GLuint>();

        return j;
    }
};


#endif //__CUBE_H__