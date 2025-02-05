

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
  void renderShadow();

  const float* GetVertexTerrain();

  //Save data
  nlohmann::json serialize() const;

  // ----

  GLuint VAO, VBO, EBO;

  int num_rows_, num_cols_;
  float height_mult_, size_;
  bool is_centered_, use_perlin_noise_;

private:

  int num_vertex_;
  std::vector<float> vertex_terrain_;
  std::vector<float> normals_terrain_;
  std::vector<float> uv_terrain_;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture tex;
  Texture height_map;

};


#endif //__TERRAIN_H__