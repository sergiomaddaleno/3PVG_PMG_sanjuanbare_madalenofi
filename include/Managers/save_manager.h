
#ifndef __SAVE_MANAGER__
#define __SAVE_MANAGER__

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include <vector>

#include "Render/light.h"

class ECSManager;
class RenderSystem;
class Shader;

class SaveManager {

public:
	nlohmann::json serialize(ECSManager& ecs);
	void deSerialize(ECSManager& ecs, const nlohmann::json& j,RenderSystem& rs,Shader& shader);

	void saveToFile(ECSManager& ecs, const char* filename);
	void loadFromFile(ECSManager& ecs, const char* filename,RenderSystem& rs,Shader& shader);
	
	glm::vec3 parseVec3(const nlohmann::json& j) {
		return glm::vec3(j[0], j[1], j[2]);
	}

	glm::quat parseQuat(const nlohmann::json& j) {
		return glm::quat(j[3], j[0], j[1], j[2]);
	}

	std::vector<nlohmann::json> entities_;

private:

	LightInfo li;

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
};


#endif //__SAVE_MANAGER__