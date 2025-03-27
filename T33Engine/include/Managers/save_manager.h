
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
class Skybox;

/**
 * @class SaveManager
 * @brief Manages saving and loading of scene data.
 */
class SaveManager {

public:

	SaveManager(ECSManager& ecs, RenderSystem& rs);
	~SaveManager();
	
	void saveToFile(std::string filename);
	void loadFromFile(std::string filename);
	std::string newFile();

	std::string getCurrentScene() { return current_scene_file; }
	void setCurrentScene(std::string scene_file) { current_scene_file = scene_file; }

private:

	nlohmann::json serialize(Skybox& skybox);
	void deSerialize(const nlohmann::json& j, Skybox& skybox);

	std::string current_scene_file;
	int number_of_scenes;

	ECSManager& ecs_;
	RenderSystem& rs_;
};


#endif // !__SAVE_MANAGER__