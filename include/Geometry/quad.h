

#ifndef __QUAD_H__
#define __QUAD_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/texture.h"
#include <memory>
#include <vector>

#include <../nlohmann/json.hpp>

class Quad {
 public:
     Quad();
    ~Quad();
 
   void init(const float quad_size = 1.0f);

   void bind();
   void render();

  GLuint VAO = 0, VBO = 0, EBO = 0;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture tex;
  float size_ = 0.0f;

  //Save data
  nlohmann::json serialize() const {
      nlohmann::json json_data;

      json_data["Texture"].push_back(tex.serialize());
      json_data["size"] = size_;

      return json_data;
  }

};


#endif //__QUAD_H__