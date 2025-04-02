#include "Managers/resource_manager.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"
#include "Geometry/surface.h"

std::unordered_map<std::string, std::shared_ptr<std::string>> ResourceManager::file_path_map;
std::unordered_map<std::string, std::shared_ptr<Mesh>> ResourceManager::mesh_map;

std::shared_ptr<Mesh> ResourceManager::FindMesh(const std::shared_ptr<std::string>& file_path, const std::shared_ptr<std::string>& texture_path)
{
	auto key = *file_path + *texture_path;
	auto it = mesh_map.find(key);
	if (it != mesh_map.end()) {
		return it->second;
	}

	return nullptr;
}

void ResourceManager::StoreMesh(const std::shared_ptr<std::string>& file_path, const std::shared_ptr<std::string>& texture_path, std::shared_ptr<Mesh> mesh)
{
	auto key = *file_path + *texture_path;
	if (mesh_map.find(key) == mesh_map.end()) {
		mesh_map[key] = mesh;
	}
}

void ResourceManager::clearMaps() {
    std::cout << "Clearing mesh_map and file_path_map..." << std::endl;

    mesh_map.clear();
    file_path_map.clear();

    // Print size after clearing to verify
    std::cout << "mesh_map size after clear: " << mesh_map.size() << std::endl;
    std::cout << "file_path_map size after clear: " << file_path_map.size() << std::endl;

    // Explicitly clear all render entity maps
    GetRenderEntityMap<Mesh>().clear();
    GetRenderEntityMap<Sphere>().clear();
    GetRenderEntityMap<Terrain>().clear();
    GetRenderEntityMap<Quad>().clear();
    GetRenderEntityMap<Surface>().clear();
    GetRenderEntityMap<Cube>().clear();

    // Optionally reset shared pointers to release resources
    for (auto& pair : mesh_map) {
        pair.second.reset();
    }
    for (auto& pair : file_path_map) {
        pair.second.reset();
    }

    // Print final sizes after reset
    std::cout << "mesh_map size after reset: " << mesh_map.size() << std::endl;
    std::cout << "file_path_map size after reset: " << file_path_map.size() << std::endl;
}

