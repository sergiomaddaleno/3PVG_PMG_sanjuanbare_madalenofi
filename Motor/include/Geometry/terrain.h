/**
 * @file terrain.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __TERRAIN__
#define __TERRAIN__ 1

#include <iostream>
#include <memory>
#include <vector>

#include "Geometry/wave.h"
#include "Core/texture.h"
#include "Core/camera.h"
#include "Core/shader.h"

#include <../deps/glm/glm.hpp>
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Terrain
  * @brief Represents a 3D terrain.
  */
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

  std::vector<float> GetVertexTerrain();

  /**
  * @brief Initializes the world grid.
  */
  void initGrid(const int num_cols = 70, const int num_rows = 70);
  void drawGrid(Camera cam = Camera());

  nlohmann::json serialize() const;

  // ----

  GLuint VAO, VBO, EBO;

  int num_rows_, num_cols_;
  float height_mult_, size_;
  bool is_centered_, use_perlin_noise_;

  Texture texture_;
  bool use_texture_;
  glm::vec3 color_;
  std::string texture_path_;
  std::string heightmap_path_;

  GerstnerWave wave_;

  glm::vec3 grid_color_;
  float grid_scale_;

private:

  int num_vertex_;
  std::vector<float> vertex_terrain_;
  std::vector<float> normals_terrain_;
  std::vector<float> uv_terrain_;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture height_map_;

  Shader gridShader;

};

#endif // !__TERRAIN__