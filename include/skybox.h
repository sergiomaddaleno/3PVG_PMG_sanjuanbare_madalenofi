
#ifndef SKY
#define SKY

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/glm/glm.hpp>

struct RenderData;

static const glm::vec3 gMesh[] = {
  {-1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f, -1.0f},
  {1.0f,  1.0f, -1.0f},
  {-1.0f,  1.0f, -1.0f},

  {-1.0f, -1.0f,  1.0f},
  {1.0f, -1.0f,  1.0f},
  {1.0f,  1.0f,  1.0f},
  {-1.0f,  1.0f,  1.0f},

  {-1.0f,  1.0f,  1.0f},
  {-1.0f,  1.0f, -1.0f},
  {-1.0f, -1.0f, -1.0f},
  {-1.0f, -1.0f,  1.0f},

  {1.0f,  1.0f,  1.0f},
  {1.0f,  1.0f, -1.0f},
  {1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f,  1.0f},

  {-1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f,  1.0f},
  {-1.0f, -1.0f,  1.0f},

  {-1.0f,  1.0f, -1.0f},
  {1.0f,  1.0f, -1.0f},
  {1.0f,  1.0f,  1.0f},
  {-1.0f,  1.0f,  1.0f},
};

#define _CF(A)   A,A+1,A+2,A+2,A+3,A+0
#define _SF(A)   A,A+2,A+1,A+2,A+0,A+3  

static const unsigned int gIndices[] = {
  _SF(0),
  _CF(4),
  _CF(8),
  _SF(12),
  _CF(16),
  _SF(20)
};

class Skybox {

public:
	Skybox();

	void init();

	void render(RenderData& rd);

private:

	float skybox_angle;

	GLuint VAO, VBO, EBO;
};



#endif //SKY