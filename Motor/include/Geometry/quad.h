

#ifndef __QUAD_H__
#define __QUAD_H__ 1

#include <memory>
#include <vector>

#include "Core/texture.h"
#include "Geometry/wave.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Quad {
 public:
     Quad();
    ~Quad();
 
   void init(const float quad_size = 1.0f);

   void bind();
   void draw();
   void setTexture(std::string file_path);

   //Save data
   nlohmann::json serialize() const;

   //---

  GLuint VAO, VBO, EBO;

  float size_;
  Texture texture_;
  bool use_texture_;
  glm::vec3 color_;
  std::string texture_path_;

  GerstnerWave wave_;

  std::vector<int> order_;
  std::vector<float> data_;
};


#endif //__QUAD_H__