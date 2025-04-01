#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

#include "../Components/render_component.h"
#include "../Render/light.h"
#include "Core/texture.h"

#include <unordered_map>
#include <memory>

class ResourceManager {

public:

	static ResourceManager& GetInstance() {
		static ResourceManager instance;
		return instance;
	}

	template<typename T>
	std::shared_ptr<T> GetRenderEntity(RenderType type) {
		auto& entity_map = GetRenderEntityMap<T>();

		auto it = entity_map.find(type);
		if (it != entity_map.end()) {
			return it->second;
		}

		auto new_entity = std::make_shared<T>();
		entity_map[type] = new_entity;

		return new_entity;
	}

	template<typename T>
	std::shared_ptr<T> GetLightEntityMap(Light::LightType type) {
		auto& entity_map = GetEntityMap<T>();

		auto it = entity_map.find(type);
		if (it != entity_map.end()) {
			return it->second;
		}

		auto new_entity = std::make_shared<T>();
		entity_map[type] = new_entity;

		return new_entity;
	}

	std::shared_ptr<std::string> GetFilePathMap(std::string file_path) {
		auto& file_map = GetFilePath();

		auto it = file_map.find(file_path);
		if (it != file_map.end()) {
			return it->second;
		}

		auto new_file = std::make_shared<std::string>();
		file_map[file_path] = new_file;

		return new_file;
	}

private:
	ResourceManager() = default;
	~ResourceManager() = default;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	template<typename T>
	static std::unordered_map<RenderType, std::shared_ptr<T>>& GetRenderEntityMap() {
		static std::unordered_map<RenderType, std::shared_ptr<T>> entity_map;
		return entity_map;
	}

	template<typename T>
	static std::unordered_map<Light::LightType, std::shared_ptr<T>>& GetLightEntityMap() {
		static std::unordered_map<Light::LightType, std::shared_ptr<T>> entity_map;
		return entity_map;
	}

	static std::unordered_map<std::string, std::shared_ptr<std::string>>& GetFilePath() {
		static std::unordered_map<std::string, std::shared_ptr<std::string>> file_path_map;
		return file_path_map;
	}

};

#endif // !__RESOURCE_MANAGER__
