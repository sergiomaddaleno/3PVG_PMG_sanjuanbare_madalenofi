

#ifndef __TERRAIN_H__
#define __TERRAIN_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <vector>
#include "Core/texture.h"

#include <../deps/glm/glm.hpp>
#include <../nlohmann/json.hpp>

class Terrain {
 public:
     Terrain();
    ~Terrain();
 
  void init(const int num_cols = 2, const int num_rows = 2, 
            const float height_mult = 1.0f, const float size = 1.0f,
            const bool is_centered = true, const bool use_perlin_noise = true);  //false for heightmap

  void bind();

  void render();

  float* GetVertexTerrain();

  GLuint VAO = 0, VBO = 0, EBO = 0;

  int num_rows_=0;
  int num_cols_=0;
  float height_mult_ = 0.0f;
  float size_ = 0.0f;
  bool is_centered_ = true;
  bool use_perlin_noise_ = true;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture tex;
  Texture height_map;

  //Save data
  nlohmann::json serialize() const {
      nlohmann::json json_data;

      json_data["Texture"].push_back(tex.serialize());
      json_data["num_rows"] = num_rows_;
      json_data["num_cols"] = num_cols_;
      json_data["height_mult"] = height_mult_;
      json_data["size"] = size_;
      json_data["is_centered"] = is_centered_;
      json_data["use_perlin_noise"] = use_perlin_noise_;

      return json_data;
  }

};


#endif //__TERRAIN_H__