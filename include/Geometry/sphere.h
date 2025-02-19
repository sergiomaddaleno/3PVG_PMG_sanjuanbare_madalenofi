

#ifndef __SPHERE_H__
#define __SPHERE_H__ 1

#include <memory>
#include <vector>

#include "Core/texture.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Sphere {
 public:
     Sphere();
    ~Sphere();
 
  void init(const float sphere_size = 1.0f, const int num_heights = 20, const int num_revs = 20);

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

  // --

  int num_vertex_;
  std::vector<float> vertex_sphere_;
  std::vector<float> normals_sphere_;
  std::vector<float> uv_sphere_;

  std::vector<int> order_;
  std::vector<float> data_;
};


#endif //__SPHERE_H__