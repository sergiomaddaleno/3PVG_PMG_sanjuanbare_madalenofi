/**
 * @file sphere.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SPHERE__
#define __SPHERE__ 1

#include <memory>
#include <vector>

#include "Core/texture.h"
#include "Geometry/wave.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Sphere
  * @brief Represents a 3D sphere.
  */
class Sphere {
 public:
     Sphere();
    ~Sphere();
 
  void init(const float sphere_size = 1.0f, const int num_heights = 20, const int num_revs = 20);

  void bind();
  void draw();
  void setTexture(std::string file_path);
  std::vector<float> GetVertexSphere();

  nlohmann::json serialize() const;

  // --
  GLuint VAO, VBO, EBO;

  int num_heights_, num_revs_;
  float size_;
  Texture texture_;

  GerstnerWave wave_;
  // --

  int num_vertex_;
  std::vector<float> vertex_sphere_;
  std::vector<float> normals_sphere_;
  std::vector<float> uv_sphere_;

  std::vector<int> order_;
  std::vector<float> data_;
  std::string texture_path_;
  bool use_texture_;
  glm::vec3 color_;

};


#endif // !__SPHERE__