

#ifndef __QUAD_H__
#define __QUAD_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

class Quad {
 public:
     Quad();
    ~Quad();
 
   void init(const float quad_size = 1.0f);

  GLuint VAO = 0, VBO = 0, EBO = 0;


  std::vector<int> order_;
  std::vector<float> data_;




};


#endif //__QUAD_H__