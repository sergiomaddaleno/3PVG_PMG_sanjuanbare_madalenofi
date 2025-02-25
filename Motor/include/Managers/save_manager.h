
#ifndef _SAVE_MANAGER_H_
#define _SAVE_MANAGER_H_

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include <vector>

#include <../nlohmann/json.hpp>

#include "Render/light.h"

class ECSManager;
class RenderSystem;
class Shader;
class Camera;
class Skybox;

class SaveManager {

public:

	SaveManager();
	~SaveManager();
	
	void saveToFile(std::string filename);
	void loadFromFile(std::string filename);
	void newFile();

	std::string getCurrentScene() { return current_scene_file; }
	void setCurrentScene(std::string scene_file) { current_scene_file = scene_file; }

	glm::vec3 parseVec3(const nlohmann::json& j);
	glm::quat parseQuat(const nlohmann::json& j);

private:

	nlohmann::json serialize(ECSManager& ecs, Camera& cam, Skybox& skybox);
	void deSerialize(const nlohmann::json& j, ECSManager& ecs, RenderSystem& rs, Camera& cam, Skybox& skybox);

	std::string current_scene_file;
	int number_of_scenes;

	//---

	Light li;

	int terrain_num_rows;
	int terrain_num_cols;
	float terrain_height_mult;
	float terrain_size;
	bool terrain_is_centered;
	bool terrain_use_perlin_noise;
	std::string terrain_tex_path;

	int sphere_num_heights;
	int sphere_num_revs;
	float sphere_size;
	std::string sphere_tex_path;

	float cube_size;
	bool cube_8v;
	bool cube_24v;
	std::string cube_tex_path;

	float quad_size;
	std::string quad_tex_path;

	char* dds_file;
	char* sb_vertex;
	char* sb_fragment;
	float skybox_angle;
};


#endif //_SAVE_MANAGER_H_