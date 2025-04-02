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

	static ResourceManager& GetInstance();

	std::shared_ptr<std::string> FindFilePath(const std::string& file_path);

	std::shared_ptr<Mesh> FindMesh(const std::shared_ptr<std::string>& file_path, const std::shared_ptr<std::string>& texture_path);

	void StoreMesh(const std::shared_ptr<std::string>& file_path, const std::shared_ptr<std::string>& texture_path, std::shared_ptr<Mesh> mesh);

	void clearMaps();

private:
	ResourceManager() = default;
	~ResourceManager() = default;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	/*template<typename T>
	static std::unordered_map<RenderType, std::shared_ptr<T>>& GetRenderEntityMap() {
		static std::unordered_map<RenderType, std::shared_ptr<T>> entity_map;
		return entity_map;
	}*/

	static std::unordered_map<std::string, std::shared_ptr<std::string>> file_path_map;
	static std::unordered_map<std::string, std::shared_ptr<Mesh>> mesh_map;

};

#endif // !__RESOURCE_MANAGER__
