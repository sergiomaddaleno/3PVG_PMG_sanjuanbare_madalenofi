
#ifndef __SAVE_MANAGER__
#define __SAVE_MANAGER__

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include <vector>

#include "Render/light.h"

class ECSManager;
class RenderSystem;
class Shader;
class Camera;
class Skybox;

class SaveManager {

public:
	nlohmann::json serialize(ECSManager& ecs, Camera& cam, Skybox& skybox);
	void deSerialize(const nlohmann::json& j, ECSManager& ecs, RenderSystem& rs, Camera& cam, Skybox& skybox);

	void saveToFile(std::string filename, ECSManager& ecs, Camera& cam, Skybox& skybox);
	void loadFromFile(std::string filename, ECSManager& ecs, RenderSystem& rs, Camera& cam, Skybox& skybox);
	void newFile();

	std::string getCurrentScene() { return current_scene_file; }
	void setCurrentScene(std::string scene_file) { current_scene_file = scene_file; }

	glm::vec3 parseVec3(const nlohmann::json& j);
	glm::quat parseQuat(const nlohmann::json& j);

private:

	std::string current_scene_file;
	int number_of_scenes = 0;

	//---

	Light li;

	int terrain_num_rows = 0;
	int terrain_num_cols = 0;
	float terrain_height_mult = 0.0f;
	float terrain_size = 0.0f;
	bool terrain_is_centered = true;
	bool terrain_use_perlin_noise = true;

	int sphere_num_heights = 0;
	int sphere_num_revs = 0;
	float sphere_size = 0.0f;

	float cube_size = 0.0f;
	bool cube_8v = false;
	bool cube_24v = false;

	float quad_size = 0.0f;

	char* dds_file;
	char* sb_vertex;
	char* sb_fragment;
	float skybox_angle;
};


#endif //__SAVE_MANAGER__