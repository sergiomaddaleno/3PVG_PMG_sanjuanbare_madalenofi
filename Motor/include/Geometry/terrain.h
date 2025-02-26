#ifndef __TERRAIN_H__
#define __TERRAIN_H__ 1

#include <iostream>
#include <memory>
#include <vector>

#include "Geometry/wave.h"
#include "Core/texture.h"
#include <../deps/glm/glm.hpp>
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Terrain {
 public:
     Terrain();
    ~Terrain();
 
  void init(const int num_cols = 2, const int num_rows = 2, 
            const float height_mult = 1.0f, const float size = 1.0f,
            const bool is_centered = true, 
            const bool use_perlin_noise = true);  //false for heightmap

  void bind();
  void draw();
  void setTexture(std::string file_path);

  void setHeightMap(std::string file_path);

  const float* GetVertexTerrain();

  //Save data
  nlohmann::json serialize() const;

  // ----

  GLuint VAO, VBO, EBO;

  int num_rows_, num_cols_;
  float height_mult_, size_;
  bool is_centered_, use_perlin_noise_;

  Texture texture_;
  bool use_texture_;
  std::string texture_path_;
  std::string heightmap_path_;

  GerstnerWave wave_;

private:

  int num_vertex_;
  std::vector<float> vertex_terrain_;
  std::vector<float> normals_terrain_;
  std::vector<float> uv_terrain_;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture height_map_;

};

#endif //__TERRAIN_H__