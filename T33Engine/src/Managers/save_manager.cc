
#include "Managers/save_manager.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"
#include "Core/camera.h"
#include "Core/mesh.h"
#include "Render/light.h"
#include "Render/skybox.h"

#include "Components/components.h"

#include <../nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "utils.h"

SaveManager::SaveManager(ECSManager& ecs_, RenderSystem& rs_)
    : current_scene_file(""),
    number_of_scenes(0),
    ecs_(ecs_),
    rs_(rs_)
{
}

SaveManager::~SaveManager()
{
}

nlohmann::json SaveManager::serialize()
{
    nlohmann::json j;
    j["Entities"] = nlohmann::json::array();

    auto ri = ecs_.getComponentList<RenderComponent>().begin();
    auto ti = ecs_.getComponentList<TransformComponent>().begin();
    auto li = ecs_.getComponentList<LightComponent>().begin();
    auto ni = ecs_.getComponentList<IdentifierComponent>().begin();
    auto hi = ecs_.getComponentList<HierarchyComponent>().begin();
    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto si = ecs_.getComponentList<SkyboxComponent>().begin();

    auto riend = ecs_.getComponentList<RenderComponent>().end();
    auto tiend = ecs_.getComponentList<TransformComponent>().end();
    auto liend = ecs_.getComponentList<LightComponent>().end();
    auto niend = ecs_.getComponentList<IdentifierComponent>().end();
    auto hiend = ecs_.getComponentList<HierarchyComponent>().end();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();
    auto siend = ecs_.getComponentList<SkyboxComponent>().end();

    size_t max_components = std::max({ 
      ecs_.getComponentList<CameraComponent>().size(),
      ecs_.getComponentList<RenderComponent>().size(),
      ecs_.getComponentList<TransformComponent>().size(),
      ecs_.getComponentList<LightComponent>().size(),
      ecs_.getComponentList<IdentifierComponent>().size(),
      ecs_.getComponentList<SkyboxComponent>().size(),
      ecs_.getComponentList<HierarchyComponent>().size()
    });

    for (size_t i = 0; i < max_components; ++i) {
        nlohmann::json entity_json;

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

        if (ni != niend) {
            if (ni->has_value()) {
                entity_json["Identifier_component"] = (*ni)->serialize();
            }
            ++ni;
        }

        if (hi != hiend) {
            if (hi->has_value()) {
                entity_json["Hierarchy_component"] = (*hi)->serialize();
            }
            ++hi;
        }

        if (ci != ciend) {
            if (ci->has_value()) {
                entity_json["Camera_component"] = (*ci)->serialize();
            }
            ++ci;
        }

        if (si != siend) {
            if (si->has_value()) {
                entity_json["Skybox_component"] = (*si)->serialize();
            }
            ++si;
        }

        if (!entity_json.empty()) {
            j["Entities"].push_back(entity_json);
        }
    }

    return j;
}

void SaveManager::deSerialize(const nlohmann::json& j) {

    ecs_.remove_entities();
    rs_.entity_ = 0;

    rs_.entity_counts_ = {};
    
    //-----------------------------------------------------------

    for (const auto& entity : j["Entities"]) {

        int entity_id = entity["Identifier_component"]["Entity_id"].get<int>();

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

            switch (renderType) {
            case RenderType::RT_Mesh: {
                std::string mesh_path = entity["Render_component"]["Mesh"].value("Mesh_path", "");
                std::string texture_folder = entity["Render_component"]["Mesh"].value("Texture_folder", "");

                rs_.createMesh(mesh_path, texture_folder, tc);
                break;
            }

            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v: {
                const auto& cube = entity["Render_component"]["Cube"];

                if (cube["cube_8v"]) rs_.createCube8v(tc, cube["size"], cube["Is_centered"]);
                if (cube["cube_24v"]) rs_.createCube24v(tc, cube["size"]);

                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                render_comp->cube_->setTexture(cube["Texture_path"]);
                break;
            }

            case RenderType::RT_Quad: {
                const auto& quad = entity["Render_component"]["Quad"];

                rs_.createQuad(tc, quad["size"]);

                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                render_comp->quad_->setTexture(quad["Texture_path"]);
                render_comp->quad_->setNormalMap(quad["NormalMap_path"]);
                render_comp->quad_->setParallaxMap(quad["ParallaxMap_path"]);

                break;
            }

            case RenderType::RT_Sphere: {
                const auto& sphere = entity["Render_component"]["Sphere"];

                rs_.createSphere(tc, sphere["num_heights"], sphere["num_revs"], sphere["size"]);
                
                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                render_comp->sphere_->setTexture(sphere["Texture_path"]);
                break;
            }

            case RenderType::RT_Surface: {
                const auto& surface = entity["Render_component"]["Surface"];

                std::vector<float> surface_points;
                for (const auto& point : surface["Points"]) {
                    surface_points.push_back(point[0].get<float>());
                    surface_points.push_back(point[1].get<float>());
                    surface_points.push_back(point[2].get<float>());
                }

                rs_.createSurface(surface_points, tc, surface["num_heights"], surface["num_revs"]);

                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                render_comp->surface_->setTexture(surface["Texture_path"]);
                break;
            }

            case RenderType::RT_Terrain: {
                const auto& terrain = entity["Render_component"]["Terrain"];

                rs_.createTerrain(tc,
                    terrain["num_cols"],
                    terrain["num_rows"],
                    terrain["height_mult"],
                    terrain["size"],
                    terrain["is_centered"],
                    terrain["use_perlin_noise"]);
                
                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                render_comp->terrain_->setTexture(terrain["Texture_path"]);
                render_comp->terrain_->setHeightMap(terrain["Heightmap_path"]);

                render_comp->terrain_->init(terrain["num_cols"], terrain["num_rows"], terrain["height_mult"],
                    terrain["size"], terrain["is_centered"], terrain["use_perlin_noise"]);

                break;
            }

            case RenderType::RT_Water: {
                const auto& water = entity["Render_component"]["Water"];

                rs_.createWater(tc,
                    water["num_cols"],
                    water["num_rows"],
                    water["size"],
                    water["is_centered"]);

                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                render_comp->water_->setTexture(water["Texture_path"]);
                break;
            }
            }
        }

        // Light data ------------------------------------------------------

        if (entity.contains("Light_component")) {
            const auto& lc = entity["Light_component"]["light"];

            Light li;

            li.pos_ = parseVec3(lc["Position"][0]);
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
            li.spot_angle_ = lc["Spot_angle"][0].get<float>();
            li.spot_softness_ = lc["Spot_softness"][0].get<float>();

            li.type_ = lc["Type"].get<Light::LightType>();

            // --

            int lightType = -1;
            lightType = lc.value("Type", -1);

            switch (lightType) {
            case Light::LightType::LT_Spot:
                rs_.createSpotLight(li, false);
                break;
            case Light::LightType::LT_Point:
                rs_.createPointLight(li, false);
                break;
            case Light::LightType::LT_Directional:
                rs_.createDirectionalLight(li, false);
                break;
            }
        }

        // ---

        if (entity.contains("Identifier_component")) {

            auto ni = ecs_.getComponentList<IdentifierComponent>().begin();
            auto niend = ecs_.getComponentList<IdentifierComponent>().end();
            for (const auto& entity : j["Entities"]) {
                if (entity.contains("Identifier_component")) {
                    const auto& nc = entity["Identifier_component"];

                    if (ni != niend) {
                        if (ni->has_value()) {
                            auto& name_comp = *ni;
                            name_comp->name = nc["Entity_name"];
                            name_comp->id = nc["Entity_id"];
                            ++ni;
                        }
                    }
                }
            }
        }

        if (entity.contains("Hierarchy_component")) {

            auto hi = ecs_.getComponentList<HierarchyComponent>().begin();
            auto hiend = ecs_.getComponentList<HierarchyComponent>().end();
            for (const auto& entity : j["Entities"]) {
                if (entity.contains("Hierarchy_component")) {
                    const auto& hc = entity["Hierarchy_component"];

                    if (hi != hiend) {
                        if (hi->has_value()) {
                            auto& hierarchy_comp = *hi;

                            hierarchy_comp->clearChildren();
                            hierarchy_comp->clearParents();

                            std::vector<int> vec_child = hc["Child"].get<std::vector<int>>();
                            hierarchy_comp->setChildren(vec_child);
                            
                            std::vector<int> vec_parent = hc["Parent"].get<std::vector<int>>();
                            hierarchy_comp->setParents(vec_parent);

                            ++hi;
                        }
                    }
                }
            }
        }

        if (entity.contains("Camera_component")) {

            const auto& cc = entity["Camera_component"]["camera"];

            Camera cam;
            cam.deserialize(cc);

            rs_.addCamera(cam);
        }

        if (entity.contains("Skybox_component")) {

            const auto& sc = entity["Skybox_component"]["skybox"];

            Skybox sky;
            sky.deserialize(sc);

            rs_.addSkybox(sky);
        }
    }

    rs_.init();
}

void SaveManager::saveToFile(std::string filename)
{
    nlohmann::json j = serialize();
    std::ofstream file(filename);

    if (file.is_open()) {
        file << j.dump(1);
        file.close();
        
        printf("Saving in %s\n", filename.c_str());
    }
    else {
        printf("Unable to open file\n");
    }
}

void SaveManager::loadFromFile(std::string filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        filename = newFile();
        file.open(filename);
    }

    current_scene_file = filename;

    nlohmann::json j;
    file >> j;
    file.close();

    deSerialize(j);
    printf("Loaded from %s\n", filename.c_str());
}

std::string SaveManager::newFile()
{
    std::string directory = std::filesystem::absolute("../stages/saves/").string();
    std::filesystem::path dirPath(directory);

    if (!std::filesystem::exists(dirPath)) std::filesystem::create_directories(dirPath);

    std::string filename = directory + "scene_" + std::to_string(number_of_scenes) + ".json";
    std::ofstream file(filename);

    if (file.is_open()) {
        nlohmann::json defaultJson = {
            {"Entities", nlohmann::json::array()} 
        };

        file << defaultJson.dump(4);
        file.close();

        printf("New file created\n");
    }
    else {
        printf("Unable to create file\n");
    }

    number_of_scenes++;

    return filename;
}


