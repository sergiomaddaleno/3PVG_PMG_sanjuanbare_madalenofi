
#ifndef __SKYBOX__
#define __SKYBOX__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <../deps/glm/glm.hpp>

#include "camera.h"
#include "Core/shader.h"
#include "Core/texture.h"
#include "../Systems/ecs_manager.h"

#include "../deps/nlohmann/json.hpp"

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

	void loadDDS(const char* dds_file, const char* vertex_file, const char* fragment_file);
	void loadCubeMap(std::vector<std::string> faces_files, const char* vertex_file, const char* fragment_file);

	void init();

	void renderDDS(Camera cam = Camera());
	void renderCubeMap(Camera cam = Camera());

	nlohmann::json serialize() const;

	std::string dds_file_;
	std::vector<std::string> faces_files_; //6 faces

	std::string vertex_file_;
	std::string fragment_file_;
	float skybox_angle_;

private:
	bool loadDDSFile(const char* dds_file);
	
	Shader shader_;
	Texture texture_;

	GLuint cubeVAO, skyboxVAO, VBO, EBO;
};



#endif //__SKYBOX__