

#ifndef __GEOMETRY_CUSTOM_TERRAIN_H__
#define __GEOMETRY_CUSTOM_TERRAIN_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

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

  std::vector<int> elements_buffer;
  std::vector<int> order_buffer;


};


#endif //__GEOMETRY_CUSTOM_TERRAIN_H__