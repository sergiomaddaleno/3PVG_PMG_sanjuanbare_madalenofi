
#ifndef __SAVE_MANAGER__
#define __SAVE_MANAGER__

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include <vector>

#include "../deps/nlohmann/json.hpp"

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
};


#endif //__SAVE_MANAGER__