
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

class Skybox {

public:
	Skybox();
	~Skybox();

	void loadDDS(const char* dds_file, const char* vertex_file, const char* fragment_file);
	void loadCubeMap(std::vector<std::string> faces_files, const char* vertex_file, const char* fragment_file);

	void initDDS();
	void initcubeMap();

	void render(Camera cam = Camera());

	nlohmann::json serialize() const;

	bool use_cubemap;

	//6 faces, use this order : 
	//right, left, top, bottom, front, back
	std::vector<std::string> faces_files_; 
	std::string cubemap_vertex_file_;
	std::string cubemap_fragment_file_;

	std::string dds_file_;
	std::string dds_vertex_file_;
	std::string dds_fragment_file_;
	float dds_angle_;
	
private:
	bool loadDDSFile(const char* dds_file);
	
	void renderCubeMap(Camera cam = Camera());
	void renderDDS(Camera cam = Camera());

	Shader dds_shader, cubemap_shader;
	Texture dds_texture_, cubemap_texture_;

	GLuint skyboxVAO, VBO, EBO;
	GLuint cubemapVAO, cubemapVBO;
};



#endif //__SKYBOX__