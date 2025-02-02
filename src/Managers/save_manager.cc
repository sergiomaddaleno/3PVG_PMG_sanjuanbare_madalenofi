
#include "Managers/save_manager.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"
#include "Core/mesh.h"
#include "Render/light.h"

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
    j["Entities"] = nlohmann::json::array();

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto li = ecs.getComponentList<LightComponent>().begin();

    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();
    auto liend = ecs.getComponentList<LightComponent>().end();

    int counter = 0;
    size_t max_components = std::max({ ecs.getComponentList<RenderComponent>().size(),
                                      ecs.getComponentList<TransformComponent>().size(),
                                      ecs.getComponentList<LightComponent>().size() });

    for (size_t i = 0; i < max_components; ++i) {
        nlohmann::json entity_json;
        entity_json["Entity_id"] = counter;

        if (ri != riend) {
            if (ri->has_value()) {
                entity_json["Render_component"] = (*ri)->serialize();
            }
            ++ri;
        }

        if (ti != tiend) {
            if (ti->has_value()) {
                entity_json["Transform_component"] = (*ti)->serialize();
            }
            ++ti;
        }

        if (li != liend) {
            if (li->has_value()) {
                entity_json["Light_component"] = (*li)->serialize();
            }
            ++li;
        }

        if (!entity_json.empty()) {
            j["Entities"].push_back(entity_json);
        }

        ++counter;
    }

    return j;
}


void SaveManager::deSerialize(ECSManager& ecs, const nlohmann::json& j, RenderSystem& rs, Shader& shader) {

    ecs.remove_entities();
    rs.number_lights = 0;
    //rs.createDirectionalLight(ecs, shader);

    for (const auto& entity : j["Entities"]) {


        if (entity.contains("Render_component")) {

            const auto& tc = entity["Transform_component"];
            glm::vec3 position = parseVec3(tc["Position"]);
            glm::quat rotation = parseQuat(tc["Rotation"]);
            glm::vec3 scale = parseVec3(tc["Scale"]);

            int renderType = -1;
            if (entity.contains("Render_component")) {
                const auto& rc = entity["Render_component"];
                renderType = rc.value("Render_type", -1);
            }

            glm::vec3 light_pos = { 0.0f, 0.0f, 0.0f }, light_dir = { 0.0f, 0.0f, 0.0f }, diff_color = { 0.0f, 0.0f, 0.0f },
                spec_color = { 0.0f, 0.0f, 0.0f }, light_color = { 0.0f, 0.0f, 0.0f }, ambient_color = { 0.0f, 0.0f, 0.0f };
            float linear_att = 0.0f, quad_att = 0.0f, constant_att = 0.0f, shininess = 0.0f, strength = 0.0f, spot_angle = 0.0f;

            if (entity.contains("Light_component")) {
                const auto& lc = entity["Light_component"]["light"];

                light_pos = position;
                light_dir = parseVec3(lc["Direction"][0]);
                diff_color = parseVec3(lc["Diffusion_color"][0]);
                spec_color = parseVec3(lc["Specular_color"][0]);
                light_color = parseVec3(lc["Light_color"][0]);
                ambient_color = parseVec3(lc["Ambient_color"][0]);

                linear_att = lc["Linear_attenuation"][0].get<float>();
                quad_att = lc["Quadratic_attenuation"][0].get<float>();
                constant_att = lc["Constant_attenuation"][0].get<float>();
                shininess = lc["Shininnes"][0].get<float>();
                strength = lc["Strength"][0].get<float>();
                spot_angle = lc["Spot_angle"][0].get<float>();

                int lightType = lc["Type"].get<int>();
            }

            switch (renderType) {
            case RenderType::RT_Mesh: {
                std::string mesh_path = entity["Render_component"]["Mesh"].value("Mesh_path", "");
                std::string texture_folder = entity["Render_component"]["Mesh"].value("Texture_folder", "");
                rs.createMesh(ecs, shader, mesh_path, texture_folder, position, scale, rotation);
                break;
            }
            case RenderType::RT_Cube:
                rs.createCube8v(ecs, shader, position, scale, rotation);
                break;
            case RenderType::RT_Quad:
                rs.createQuad(ecs, shader, position, scale, rotation);
                break;
            case RenderType::RT_Sphere:
                rs.createSphere(ecs, shader, position, scale, rotation);
                break;
            case RenderType::RT_Terrain:
                rs.createTerrain(ecs, shader, position, scale, rotation, 500, 500);
                break;

            case RenderType::RT_PointLight:
                rs.createPointLight(ecs, shader, light_pos, scale, rotation, light_color);
                break;
            case RenderType::RT_SpotLight:
                rs.createSpotLight(ecs, shader, light_pos, scale, rotation, light_color);
                break;
            }
        }
    }
    rs.initEntities(ecs);
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

