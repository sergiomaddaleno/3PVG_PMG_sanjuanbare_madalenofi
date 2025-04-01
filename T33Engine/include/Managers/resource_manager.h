#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

#include "../Components/render_component.h"
#include "../Render/light.h"
#include "Core/texture.h"
#include "Core/mesh.h"

#include <unordered_map>
#include <memory>

class ResourceManager {

public:

	static ResourceManager& GetInstance() {
		static ResourceManager instance;
		return instance;
	}

	//For al render types except Mesh
	template<typename T>
	std::shared_ptr<T> FindEntity(RenderType type) {
		auto& entity_map = GetRenderEntityMap<T>();

		auto it = entity_map.find(type);
		if (it != entity_map.end()) {
			return it->second;
		}

		auto new_entity = std::make_shared<T>();
		entity_map[type] = new_entity;

		return new_entity;
	}

	std::shared_ptr<std::string> FindFilePath(std::string file_path) {
		auto& file_map = GetFilePath();

		auto it = file_map.find(file_path);
		if (it != file_map.end()) {
			return it->second;
		}

		auto new_file = std::make_shared<std::string>(file_path);
		file_map[file_path] = new_file;

		return new_file;
	}

	//--

	std::shared_ptr<Mesh> FindMesh(const std::shared_ptr<std::string>& path) {
		auto it = mesh_entities_.find(*path);
		if (it != mesh_entities_.end()) {
			return it->second;
		}
		return nullptr;
	}

	void StoreMesh(const std::shared_ptr<std::string>& path, std::shared_ptr<Mesh> mesh) {
		mesh_entities_[*path] = mesh;
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

	static std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_entities_;

};

std::unordered_map<std::string, std::shared_ptr<Mesh>> ResourceManager::mesh_entities_;


#endif // !__RESOURCE_MANAGER__
