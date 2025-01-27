
#include "Managers/save_manager.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"

#include <../nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "utils.h"

nlohmann::json SaveManager::serialize(ECSManager& ecs)
{
    nlohmann::json j;
    j["entities"] = nlohmann::json::array();

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto li = ecs.getComponentList<LightComponent>().begin();

    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();
    auto liend = ecs.getComponentList<LightComponent>().end();

    int counter = 0;
    nlohmann::json entity_json;

    for (; ri != riend && ti != tiend; ++ri, ++ti, ++counter) {
        if (ri->has_value() && ti->has_value()) {
            entity_json.clear();

            auto& render_comp = *ri;
            auto& tranform_comp = *ti;

            entity_json["entity_id"] = counter;

            entity_json["render_component"] = render_comp->serialize();
            entity_json["transform_component"] = tranform_comp->serialize();

 
            j["entities"].push_back(entity_json);
        }
    }

    ti = ecs.getComponentList<TransformComponent>().begin();

    for (; li != liend && ti != tiend; ++li, ++ti, ++counter) {
        if (li->has_value() && ti->has_value()) {
            entity_json.clear();

            auto& light_comp = *li;
            auto& tranform_comp = *ti;

            entity_json["entity_id"] = counter;

            entity_json["light_component"] = light_comp->serialize();
            entity_json["transform_component"] = tranform_comp->serialize();

            j["entities"].push_back(entity_json);
        }
    }

    return j;
}

void SaveManager::deSerialize(ECSManager& ecs, const nlohmann::json& j, RenderSystem& rs , Shader& shader) {
    ecs.remove_entities();

    rs.number_lights = 0;
    // Iterar sobre las entidades del JSON
    for (const auto& entity : j["entities"]) {
        // Verificar si la entidad tiene un "render_component" con "render_type"
        if (entity.contains("render_component") && entity["render_component"].contains("render_type")) {
            int renderType = entity["render_component"]["render_type"];

            switch (renderType) {
            case RenderType::RT_Mesh: {
                // Verificar si tiene "entity_type"
                if (entity["render_component"].contains("entity_type")) {
                    int entityType = entity["render_component"]["entity_type"];

                    switch (entityType) {
                    case EntityType::ET_Object: {
                        std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();
                        
                        rs.createMesh(ecs, mario_mesh, shader);
                        rs.initEntities(ecs);
                    } break;

                    case EntityType::ET_PointLight: {
                        rs.createPointLight(ecs,shader );
                        rs.initEntities(ecs);
                    } break;

                    case EntityType::ET_SpotLight: {
                        rs.createSpotLight(ecs,shader );
                        rs.initEntities(ecs);
                    } break;

                    default:
                        std::cerr << "Unknown entity_type: " << entityType << std::endl;
                    }
                }
                else {
                    std::cerr << "render_component does not contain entity_type." << std::endl;
                }
            } break;

            case RenderType::RT_Terrain:
                rs.createTerrain(ecs, shader);
                rs.initEntities(ecs);
                break;

            case RenderType::RT_Sphere:
                rs.createSphere(ecs, shader);
                rs.initEntities(ecs);
                break;

            case RenderType::RT_Quad:
                rs.createQuad(ecs, shader);
                rs.initEntities(ecs);
                break;

            case RenderType::RT_Cube:
                rs.createCube24v(ecs, shader);
                rs.initEntities(ecs);
                break;

            default:
                std::cerr << "Unknown render_type: " << renderType << std::endl;
            }
        }
        else {
            std::cout << "Entity ID: " << entity["entity_id"] << " does not have a render_type." << std::endl;
        }
    }

    
}




void SaveManager::saveToFile(ECSManager& ecs, const char* filename)
{
    nlohmann::json j = serialize(ecs);
    std::ofstream file(filename);

    if (file.is_open()) {
        file << j.dump(1);
        file.close();
        std::cout << "ECS data saved to " << filename << std::endl;
    }
    else {
        std::cerr << "Unable to open file for writing: " << filename << std::endl;
    }
}

void SaveManager::loadFromFile(ECSManager& ecs, const char* filename,RenderSystem& rs,Shader& shader) {
    std::ifstream file(filename);
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        file.close();
        deSerialize(ecs, j,rs,shader);
        std::cout << "ECS data loaded from " << filename << std::endl;
    }
    else {
        std::cerr << "Unable to open file for reading: " << filename << std::endl;
    }
}

