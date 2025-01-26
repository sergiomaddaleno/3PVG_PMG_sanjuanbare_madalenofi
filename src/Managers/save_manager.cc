
#include "Managers/save_manager.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"

#include <../nlohmann/json.hpp>
#include <iostream>
#include <fstream>

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

RenderSystem rs;

void SaveManager::deSerialize(ECSManager& ecs, const nlohmann::json& j) {

    for (const auto& entity_json : j["entities"]) {

        //rs.entity_ = entity_json["entity_id"].get<size_t>();
        
        if (entity_json.contains("render_component")) {
           
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

void SaveManager::loadFromFile(ECSManager& ecs, const char* filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        file.close();
        deSerialize(ecs, j);
        std::cout << "ECS data loaded from " << filename << std::endl;
    }
    else {
        std::cerr << "Unable to open file for reading: " << filename << std::endl;
    }
}

