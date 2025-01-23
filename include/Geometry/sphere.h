

#ifndef __SPHERE_H__
#define __SPHERE_H__ 1

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

class Sphere {
 public:
     Sphere();
    ~Sphere();
 
  void init(bool sky = false, const float sphere_size = 1.0f, const int num_heights = 20, const int num_revs = 20);

  float* GetVertexSphere();

  GLuint VAO = 0, VBO = 0, EBO = 0;

  int num_heights_=0;
  int num_revs_=0;

  std::vector<int> order_;
  std::vector<float> data_;



};


#endif //__SPHERE_H__