

#ifndef __CUBE_H__
#define __CUBE_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Render/texture.h"
#include <memory>
#include <vector>

class Cube {
 public:
     Cube();
    ~Cube();
 
    void init8v(const float cube_size = 1.0f);
    void init24v(const float cube_size = 1.0f);

  GLuint VAO = 0, VBO = 0, EBO = 0;


  std::vector<int> order_;
  std::vector<float> data_;
  int type = 0;


  Texture tex;

};


#endif //__CUBE_H__