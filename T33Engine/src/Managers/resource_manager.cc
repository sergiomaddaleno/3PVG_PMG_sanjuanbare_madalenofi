#include "Managers/resource_manager.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"
#include "Geometry/surface.h"

std::unordered_map<std::string, std::shared_ptr<std::string>> ResourceManager::file_path_map;
std::unordered_map<std::string, std::shared_ptr<Mesh>> ResourceManager::mesh_map;

// For all render types except mesh
/*template<typename T>
std::shared_ptr<T> FindEntity(RenderType type) {
    auto& entity_map = GetRenderEntityMap<T>();

    auto it = entity_map.find(type);
    if (it != entity_map.end()) {
        return it->second;
    }

    auto new_entity = std::make_shared<T>();
    entity_map[type] = new_entity;

    return new_entity;
}*/

std::shared_ptr<std::string> ResourceManager::FindFilePath(const std::string& file_path)
{
    auto& file_map = file_path_map;

    auto it = file_map.find(file_path);
    if (it != file_map.end()) {
        return it->second;
    }

    auto new_file = std::make_shared<std::string>(file_path);
    file_map[file_path] = new_file;

    return new_file;
}

std::shared_ptr<Mesh> ResourceManager::FindMesh(const std::shared_ptr<std::string>& file_path,
    const std::shared_ptr<std::string>& texture_path)
{
    auto key = *file_path + "_" + *texture_path; 

    auto it = mesh_map.find(key);
    if (it != mesh_map.end()) {
        return it->second;
    }

    return nullptr;
}


void ResourceManager::StoreMesh(const std::shared_ptr<std::string>& file_path, const std::shared_ptr<std::string>& texture_path, std::shared_ptr<Mesh> mesh)
{
    auto key = *file_path + "_" + *texture_path;
    auto it = mesh_map.find(key);
    if (it == mesh_map.end() || !it->second) { 
        mesh_map[key] = mesh;
    }
}

void ResourceManager::clearMaps() {
    
    for (auto& pair : mesh_map) {
        pair.second.reset();
    }
    mesh_map.clear();

    for (auto& pair : file_path_map) {
        pair.second.reset();
    }
    file_path_map.clear();

    for (auto& pair : GetRenderEntityMap<Mesh>()) {
        pair.second.reset();
    }
    GetRenderEntityMap<Mesh>().clear();

    for (auto& pair : GetRenderEntityMap<Sphere>()) {
        pair.second.reset();
    }
    GetRenderEntityMap<Sphere>().clear();
}

