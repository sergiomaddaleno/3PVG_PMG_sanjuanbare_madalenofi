

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

  float* GetVertexSphere();

  GLuint VAO = 0, VBO = 0, EBO = 0;

  int num_heights_=0;
  int num_revs_=0;
  float size_ = 0.0f;
  bool sky_ = false;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture tex;

  //Save data
  nlohmann::json serialize() const {
      nlohmann::json json_data;

      json_data["Texture"].push_back(tex.serialize());
      json_data["num_heights"] = num_heights_;
      json_data["num_revs"] = num_revs_;
      json_data["size_"] = size_;
      json_data["sky"] = sky_;

      return json_data;
  }

};


#endif //__SPHERE_H__