
#include "Managers/save_manager.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"
#include "Render/camera.h"
#include "Core/mesh.h"
#include "Render/light.h"
#include "Render/skybox.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"
#include "Components/name_component.h"

#include "global_variables.h"

#include <../nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include "utils.h"

nlohmann::json SaveManager::serialize(ECSManager& ecs, Camera& cam, Skybox& skybox)
{
    nlohmann::json j;
    j["Entities"] = nlohmann::json::array();

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto li = ecs.getComponentList<LightComponent>().begin();
    auto ni = ecs.getComponentList<NameComponent>().begin();

    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();
    auto liend = ecs.getComponentList<LightComponent>().end();
    auto niend = ecs.getComponentList<NameComponent>().end();

    int counter = 0;
    size_t max_components = std::max({ ecs.getComponentList<RenderComponent>().size(),
      ecs.getComponentList<TransformComponent>().size(),
      ecs.getComponentList<LightComponent>().size(),
      ecs.getComponentList<NameComponent>().size()
    });

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

        if (ni != niend) {
            if (ni->has_value()) {
                entity_json["Name_component"] = (*ni)->serialize();
            }
            ++ni;
        }

        if (!entity_json.empty()) {
            j["Entities"].push_back(entity_json);
        }

        ++counter;
    }

    //-----------------

    j["Camera"] = cam.serialize(); 

    j["Skybox"] = skybox.serialize();

    return j;
}


void SaveManager::deSerialize(const nlohmann::json& j, ECSManager& ecs, RenderSystem& rs, Camera &cam, Skybox& skybox) {

    ecs.remove_entities();
    rs.number_lights = 0;

    rs.entity_mesh = 0;
    rs.entity_terrain = 0;
    rs.entity_sphere = 0;
    rs.entity_cube8v = 0;
    rs.entity_cube24v = 0;
    rs.entity_quad = 0;
    rs.entity_spotlight = 0;
    rs.entity_pointlight = 0;
    rs.entity_dirlight = 0;

    //-----------------------------------------------------------

    char* vs_entities = (char*)ReadFile("../assets/shaders/entities.vs");
    char* fs_entities = (char*)ReadFile("../assets/shaders/entities.fs");

    Shader shader;
    shader.compile(vs_entities, fs_entities);
    
    //rs.createDirectionalLight(ecs, shader);
    //-----------------------------------------------------------

    const auto& camera = j["Camera"]; 

    cam.setPosition(parseVec3(camera["Position"]));
    cam.setFov(camera["Fov"]);
    cam.setScrollSpeed(camera["Scroll_speed"]);
    cam.setSensitibity(camera["Sensitivity"]);
    cam.setSpeed(camera["Speed"]);
    cam.setAspectRatio(camera["Aspect_ratio"]);
    cam.setMinRenderDistance_(camera["Min_render_distance"]);
    cam.setMaxRenderDistance_(camera["Max_render_distance_"]);
    
    //-----------------------------------------------------------
    
    const auto& skybox_ = j["Skybox"];

    skybox.dds_file_ = skybox_["Skybox_file_path"];
    skybox.vertex_file_ = skybox_["Vertex_source"];
    skybox.fragment_file_ = skybox_["Fragment_source"];
    skybox.skybox_angle_ = skybox_["angle"];

    skybox.loadSkyBox(skybox.dds_file_.c_str(), skybox.vertex_file_.c_str(), skybox.fragment_file_.c_str());
    skybox.init();

    //-----------------------------------------------------------

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

        if (entity.contains("Name_component") && entity.contains("Entity_id")) {
            int entity_id = entity["Entity_id"];
            const auto& nc = entity["Name_component"];

            if (ecs.has_component<NameComponent>(entity_id)) {
                auto& name_comp = ecs.get_component<NameComponent>(entity_id);
                name_comp->name = nc.value("Entity_name", "Unnamed");
            }
        }
    }
    rs.initEntities(ecs);

    free(vs_entities);
    free(fs_entities);
}


void SaveManager::saveToFile(std::string filename, ECSManager& ecs, Camera& cam, Skybox& skybox)
{
    nlohmann::json j = serialize(ecs, cam, skybox);
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

void SaveManager::loadFromFile(std::string filename, ECSManager& ecs, RenderSystem& rs, Camera& cam, Skybox& skybox) {

    std::ifstream file(filename);
    if (file.is_open()) {

        current_scene_file = filename;

        nlohmann::json j;
        file >> j;
        file.close();
        deSerialize(j, ecs,rs, cam, skybox);
        
        printf("Loaded from %s\n", filename.c_str());
    }
    else {
        printf("Unable to open file\n");
    }
}

void SaveManager::newFile()
{
    std::string filename = "../saves/scene_" + std::to_string(number_of_scenes) + ".json";
    std::ofstream file(filename);

    if (file.is_open()) {
        nlohmann::json defaultJson = {
            {"Entities", nlohmann::json::array()},
            {"Camera", {
                {"Position", {0.0f, 0.0f, 0.0f}},
                {"Fov", 45.0f},
                {"Scroll_speed", 1.0f},
                {"Sensitivity", 1.0f},
                {"Speed", 0.005f},
                {"Aspect_ratio", 1.777f},
                {"Min_render_distance", 0.1f},
                {"Max_render_distance_", 1000.0f}
            }},
            {"Skybox", {
                {"Fragment_source", "../assets/shaders/skybox.fs"},
                {"Skybox_file_path", "../assets/textures/skybox/sky.dds"},
                {"Vertex_source", "../assets/shaders/skybox.vs"},
                {"angle", -90.0f }
            }}
        };

        file << defaultJson.dump(4);
        file.close();

        printf("New file created\n");
    }
    else {
        printf("Unable to create file\n");
    }

    number_of_scenes++;
}

glm::vec3 SaveManager::parseVec3(const nlohmann::json& j)
{
    return glm::vec3(j[0], j[1], j[2]);
}

glm::quat SaveManager::parseQuat(const nlohmann::json& j)
{
    return glm::quat(j[3], j[0], j[1], j[2]);
}


