

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
   void setTexture(std::string texture_path);
   void setNormalMap(std::string texture_path);

   glm::vec3* GetVertex();

   nlohmann::json serialize() const;

   //---

  GLuint VAO, VBO, EBO;

  float size_;
  Texture texture_;
  Texture normal_map_;
  bool use_texture_;
  glm::vec3 color_;

  std::string texture_path_;
  std::string normal_map_path_;

  GerstnerWave wave_;

private:

  std::vector<int> order_;
  std::vector<float> data_;

  glm::vec3 vertex[3];

};


#endif //__QUAD_H__