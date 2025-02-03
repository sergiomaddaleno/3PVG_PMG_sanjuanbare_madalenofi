

#ifndef __SPHERE_H__
#define __SPHERE_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Core/texture.h"
#include <memory>
#include <vector>

#include <../nlohmann/json.hpp>

class Sphere {
 public:
     Sphere();
    ~Sphere();
 
  void init(bool sky = false, const float sphere_size = 1.0f, const int num_heights = 20, const int num_revs = 20);

  void bind();
  void render();
  float* GetVertexSphere();

  //Save data
  nlohmann::json serialize() const;

  // --
  GLuint VAO, VBO, EBO;

  int num_heights_, num_revs_;
  float size_ = 0.0f;
  Texture tex;

  std::vector<int> order_;
  std::vector<float> data_;
};


#endif //__SPHERE_H__