
/**
 * @file save_manager.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SAVE_MANAGER__
#define __SAVE_MANAGER__

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

/**
 * @class SaveManager
 * @brief Manages saving and loading of scene data.
 */
class SaveManager {

public:

	SaveManager();
	~SaveManager();
	
	void saveToFile(std::string filename);
	void loadFromFile(std::string filename);
	std::string newFile();

	std::string getCurrentScene() { return current_scene_file; }
	void setCurrentScene(std::string scene_file) { current_scene_file = scene_file; }

	glm::vec3 parseVec3(const nlohmann::json& j);
	glm::quat parseQuat(const nlohmann::json& j);

private:

	nlohmann::json serialize(ECSManager& ecs, Camera& cam, Skybox& skybox);
	void deSerialize(const nlohmann::json& j, RenderSystem& rs, Camera& cam, Skybox& skybox);

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
	std::string heightmap_path;

	int sphere_num_heights;
	int sphere_num_revs;
	float sphere_size;
	std::string sphere_tex_path;

	float cube_size;
	bool cube_is_centered;
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


#endif // !__SAVE_MANAGER__