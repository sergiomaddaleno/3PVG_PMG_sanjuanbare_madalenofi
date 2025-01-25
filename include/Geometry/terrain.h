

#ifndef __TERRAIN_H__
#define __TERRAIN_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>
#include <vector>
#include "Render/texture.h"

#include <../deps/glm/glm.hpp>
#include <../nlohmann/json.hpp>

class Terrain {
 public:
     Terrain();
    ~Terrain();
 
  void init(const int num_cols = 2, const int num_rows = 2, 
            const float height_mult = 1.0f, const float size = 1.0f,
            const bool is_centered = true, const bool use_perlin_noise = true);  //false for heightmap

  float* GetVertexTerrain();

  GLuint VAO = 0, VBO = 0, EBO = 0;

  int num_rows_=0;
  int num_cols_=0;

  std::vector<int> order_;
  std::vector<float> data_;

  Texture tex;
  glm::vec3 rgb_ = { 0.0f, 100.0f, 0.0f };
  bool activate_color_ = true;

  //Save data
  nlohmann::json serialize() const {
      nlohmann::json json_data;

      json_data["Texture"].push_back(tex.serialize());
      json_data["VAO"] = VAO;
      json_data["VBO"] = VBO;
      json_data["EBO"] = EBO;

      return json_data;
  }

  nlohmann::json deSerialize(nlohmann::json& j) {
      
      tex.deSerialize(j["Texture"][0]);
      VAO = j["VAO"].get<GLuint>();
      VBO = j["VBO"].get<GLuint>();
      EBO = j["EBO"].get<GLuint>();

      return j;
  }

};


#endif //__TERRAIN_H__