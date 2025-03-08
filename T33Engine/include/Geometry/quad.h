/**
 * @file quad.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __QUAD__
#define __QUAD__ 1

#include <memory>
#include <vector>

#include "Core/texture.h"
#include "Geometry/wave.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Quad
  * @brief Represents a 2D quad.
  */
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
  bool use_normal_map_;

  std::string texture_path_;
  std::string normal_map_path_;

  GerstnerWave wave_;

private:

  std::vector<int> order_;
  std::vector<float> data_;

  glm::vec3 vertex[3];

};

#endif // !__QUAD__