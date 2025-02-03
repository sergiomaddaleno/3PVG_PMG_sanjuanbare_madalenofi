
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

            const auto& tc_jsonc = entity["Transform_component"];
            TransformComponent tc;

            tc.position = parseVec3(tc_jsonc["Position"]);
            tc.rotation = parseQuat(tc_jsonc["Rotation"]);
            tc.scale = parseVec3(tc_jsonc["Scale"]);

            int renderType = -1;
            if (entity.contains("Render_component")) {
                const auto& rc = entity["Render_component"];
                renderType = rc.value("Render_type", -1);
            }

            // Light data ------------------------------------------------------

            if (entity.contains("Light_component")) {
                const auto& lc = entity["Light_component"]["light"];

                li.pos_ = tc.position;
                li.dir_ = parseVec3(lc["Direction"][0]);
                li.diff_color_ = parseVec3(lc["Diffusion_color"][0]);
                li.spec_color_ = parseVec3(lc["Specular_color"][0]);
                li.light_color_ = parseVec3(lc["Light_color"][0]);
                li.ambient_color_ = parseVec3(lc["Ambient_color"][0]);

                li.linear_att = lc["Linear_attenuation"][0].get<float>();
                li.quad_att_ = lc["Quadratic_attenuation"][0].get<float>();
                li.constant_att_ = lc["Constant_attenuation"][0].get<float>();
                li.shininess_ = lc["Shininnes"][0].get<float>();
                li.strength_ = lc["Strength"][0].get<float>();
                li.spot_angle_= lc["Spot_angle"][0].get<float>();

                li.type_ = lc["Type"].get<LightType>();
            }

            // ---

            switch (renderType) {
            case RenderType::RT_Mesh: {
                std::string mesh_path = entity["Render_component"]["Mesh"].value("Mesh_path", "");
                std::string texture_folder = entity["Render_component"]["Mesh"].value("Texture_folder", "");
                rs.createMesh(ecs, shader, mesh_path, texture_folder, tc);
                break;
            }
            case RenderType::RT_Cube: {
                const auto& cube = entity["Render_component"]["Cube"];
                cube_8v = cube["cube_8v"];
                cube_24v = cube["cube_24v"];
                cube_size = cube["size"];
                if (cube_8v) rs.createCube8v(ecs, shader, tc, cube_size);
                if (cube_24v) rs.createCube24v(ecs, shader, tc, cube_size);

                break;
            }
            case RenderType::RT_Quad: {
                const auto& quad = entity["Render_component"]["Quad"];
                quad_size = quad["size"];
                rs.createQuad(ecs, shader, tc, quad_size);

                break;
            }
            case RenderType::RT_Sphere:{
                const auto& sphere = entity["Render_component"]["Sphere"];
                sphere_num_heights = sphere["num_heights"];
                sphere_num_revs = sphere["num_revs"];
                sphere_size = sphere["size"];
                rs.createSphere(ecs, shader, tc, sphere_num_heights, sphere_num_revs, sphere_size);

                break;
            }
            case RenderType::RT_Terrain: {
                const auto& terrain = entity["Render_component"]["Terrain"];
                terrain_num_rows = terrain["num_rows"];
                terrain_num_cols = terrain["num_cols"];
                terrain_height_mult = terrain["height_mult"];
                terrain_size = terrain["size"];
                terrain_is_centered = terrain["is_centered"];
                terrain_use_perlin_noise = terrain["use_perlin_noise"];
                rs.createTerrain(ecs, shader, tc, terrain_num_cols, terrain_num_rows, terrain_height_mult, terrain_size, terrain_is_centered, terrain_use_perlin_noise);

                break;
            }
            case RenderType::RT_PointLight:
                rs.createPointLight(ecs, shader, li, false);
                break;
            case RenderType::RT_SpotLight:
                rs.createSpotLight(ecs, shader, li, false);
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

