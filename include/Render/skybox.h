
#ifndef SKY
#define SKY

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/glm/glm.hpp>

#include "camera.h"
#include "Core/shader.h"
#include "Core/texture.h"
#include "../Systems/ecs_manager.h"

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
	~Skybox();

	void loadSkyBox(const char* dds_file, const char* vertex_file, const char* fragment_file);
	
	void init();

	void render(Camera cam = Camera());

private:
	
	Shader shader_;
	Texture texture_;

	float skybox_angle;

	GLuint VAO, VBO, EBO;
};



#endif //SKY