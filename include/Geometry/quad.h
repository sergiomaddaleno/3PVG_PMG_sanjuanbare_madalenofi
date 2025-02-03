

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

   //Save data
   nlohmann::json serialize() const;

   //---

  GLuint VAO, VBO, EBO;

  float size_;
  Texture tex;

  std::vector<int> order_;
  std::vector<float> data_;
};


#endif //__QUAD_H__