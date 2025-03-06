
#include "Managers/save_manager.h"
#include "Systems/ecs_manager.h"
#include "Render/render_system.h"
#include "Core/camera.h"
#include "Core/mesh.h"
#include "Render/light.h"
#include "Render/skybox.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"
#include "Components/name_component.h"

#include "global_data.h"

#include <../nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "utils.h"

SaveManager::SaveManager(ECSManager& ecs_, RenderSystem& rs_)
    : current_scene_file(""),
    number_of_scenes(0),
    li(),
    terrain_num_rows(0),
    terrain_num_cols(0),
    terrain_height_mult(0.0f),
    terrain_size(0.0f),
    terrain_is_centered(true),
    terrain_use_perlin_noise(true),
    sphere_num_heights(0),
    sphere_num_revs(0),
    sphere_size(0.0f),
    cube_size(0.0f),
    cube_is_centered(false),
    cube_8v(false),
    cube_24v(false),
    quad_size(0.0f),
    dds_file(nullptr),
    sb_vertex(nullptr),
    sb_fragment(nullptr),
    skybox_angle(0.0f),
    ecs_(ecs_),
    rs_(rs_)

{
}

SaveManager::~SaveManager()
{
}

nlohmann::json SaveManager::serialize(Camera& cam, Skybox& skybox)
{
    nlohmann::json j;
    j["Entities"] = nlohmann::json::array();

    auto ri = ecs_.getComponentList<RenderComponent>().begin();
    auto ti = ecs_.getComponentList<TransformComponent>().begin();
    auto li = ecs_.getComponentList<LightComponent>().begin();
    auto ni = ecs_.getComponentList<NameComponent>().begin();

    auto riend = ecs_.getComponentList<RenderComponent>().end();
    auto tiend = ecs_.getComponentList<TransformComponent>().end();
    auto liend = ecs_.getComponentList<LightComponent>().end();
    auto niend = ecs_.getComponentList<NameComponent>().end();

    for (size_t i = 0; i < ecs_.getEntityCount(); ++i) {
        nlohmann::json entity_json;

        entity_json["Entity_id"] = static_cast<int>(i);

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
    }

    //-----------------

    j["Camera"] = cam.serialize(); 
    j["Skybox"] = skybox.serialize();

    return j;
}

void SaveManager::deSerialize(const nlohmann::json& j, Camera &cam, Skybox& skybox) {

    ecs_.remove_entities();
    rs_.entity_ = 0;

    rs_.entity_mesh = 0;
    rs_.entity_terrain = 0;
    rs_.entity_sphere = 0;
    rs_.entity_cube8v = 0;
    rs_.entity_cube24v = 0;
    rs_.entity_quad = 0;
    rs_.entity_spotlight = 0;
    rs_.entity_pointlight = 0;
    rs_.entity_dirlight = 0;

    //-----------------------------------------------------------

    const auto& camera = j["Camera"]; 

    cam.setPosition(parseVec3(camera["Position"]));
    cam.setFov(camera["Fov"]);
    cam.setScrollSpeed(camera["Scroll_speed"]);
    cam.setSensitibity(camera["Sensitivity"]);
    cam.setSpeed(camera["Speed"]);
    cam.setAspectRatio(camera["Aspect_ratio"]);
    cam.setNearCliPlane(camera["Near_clip_plane"]);
    cam.setFarCliPlane(camera["Far_clip_plane"]);
    cam.setNearCliPlane(camera["Near_clip_plane"]);
    cam.setFarCliPlane(camera["Far_clip_plane"]);
    cam.setOrthoWidth(camera["Ortho_width"]);
    cam.setOrthoHeight(camera["Ortho_height"]);
    cam.setUsePerspectiveMatrix(camera["Use_perspective_matrix"]);
    cam.setViewDir(parseVec3(camera["View_direction"]));
    cam.initViewTarget();
    
    //-----------------------------------------------------------
    
    const auto& skybox_ = j["Skybox"];
    skybox.use_cubemap = skybox_["Use_cubemap"];

    skybox.dds_file_ = skybox_["DDS_file_path"];
    skybox.dds_fragment_file_ = skybox_["DDS_fragment_file"];
    skybox.dds_vertex_file_ = skybox_["DDS_vertex_file"];
    skybox.dds_angle_ = skybox_["DDS_angle"];

    skybox.faces_files_ = skybox_["Cubemap_file_paths"];
    skybox.cubemap_vertex_file_ = skybox_["Cubemap_vertex_file"];
    skybox.cubemap_fragment_file_ = skybox_["Cubemap_fragment_file"];

    if (skybox.use_cubemap == true) {
        skybox.loadCubeMap(skybox.faces_files_, skybox.cubemap_vertex_file_.c_str(), skybox.cubemap_fragment_file_.c_str());
        skybox.initcubeMap();
    }
    else {
        skybox.loadDDS(skybox.dds_file_.c_str(), skybox.dds_vertex_file_.c_str(), skybox.dds_fragment_file_.c_str());
        skybox.initDDS();
    }
    
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
                cube_8v = cube["cube_8v"];
                cube_24v = cube["cube_24v"];
                cube_size = cube["size"];
                cube_is_centered = cube["Is_centered"];
                cube_tex_path = cube["Texture_path"];

                if (cube_8v) rs_.createCube8v(tc, cube_size, cube_is_centered);
                if (cube_24v) rs_.createCube24v(tc, cube_size);

                int entity_id = entity["Entity_id"].get<int>();
                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                if (render_comp && render_comp->cube_) {
                    render_comp->cube_->setTexture(cube_tex_path);
                }
                break;
            }

            case RenderType::RT_Quad: {
                const auto& quad = entity["Render_component"]["Quad"];
                quad_size = quad["size"];
                quad_tex_path = quad["Texture_path"];

                rs_.createQuad(tc, quad_size);

                int entity_id = entity["Entity_id"].get<int>();
                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                if (render_comp && render_comp->quad_) {
                    render_comp->quad_->setTexture(quad_tex_path);
                }
                break;
            }

            case RenderType::RT_Sphere: {
                const auto& sphere = entity["Render_component"]["Sphere"];
                sphere_num_heights = sphere["num_heights"];
                sphere_num_revs = sphere["num_revs"];
                sphere_size = sphere["size"];
                sphere_tex_path = sphere["Texture_path"];

                rs_.createSphere(tc, sphere_num_heights, sphere_num_revs, sphere_size);
                
                int entity_id = entity["Entity_id"].get<int>();
                auto& render_comp = ecs_.get_component<RenderComponent>(entity_id);
                if (render_comp && render_comp->sphere_) {
                    render_comp->sphere_->setTexture(sphere_tex_path);
                }
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
                terrain_tex_path = terrain["Texture_path"];
                heightmap_path = terrain["Heightmap_path"];

                rs_.createTerrain(tc,
                    terrain_num_cols,
                    terrain_num_rows,
                    terrain_height_mult,
                    terrain_size,
                    terrain_is_centered,
                    terrain_use_perlin_noise);
                
                int entity_id = entity["Entity_id"].get<int>();
                auto& render_comp = ecs_.get_component<RenderComponent>(entity);
                if (render_comp && render_comp->terrain_) {
                    render_comp->terrain_->setHeightMap(heightmap_path);
                    render_comp->terrain_->setTexture(terrain_tex_path);
                }
                
                break;
            }
            }
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

        if (entity.contains("Name_component")) {

            auto ni = rs_.ecs_.getComponentList<NameComponent>().begin();
            auto niend = rs_.ecs_.getComponentList<NameComponent>().end();
            for (const auto& entity : j["Entities"]) {
                if (entity.contains("Name_component")) {
                    const auto& nc = entity["Name_component"];

                    if (ni != niend) {
                        if (ni->has_value()) {
                            auto& name_comp = *ni;
                            name_comp->name = nc["Entity_name"];
                            ++ni;
                        }
                    }
                }
            }
        }
    }
    rs_.init();
}

void SaveManager::saveToFile(std::string filename)
{
    nlohmann::json j = serialize(global.cam, global.skybox);
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

    deSerialize(j, global.cam, global.skybox);
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
            {"Entities", nlohmann::json::array()},
            {"Camera", {
                {"Position", {0.0f, 0.0f, -10.0f}},
                {"View_direction", {0.0f, 0.0f, -1.0f}},
                {"Fov", 45.0f},
                {"Scroll_speed", 1.0f},
                {"Sensitivity", 1.0f},
                {"Speed", 0.05f},
                {"Aspect_ratio", 1.777f},
                {"Near_clip_plane", 0.1f},
                {"Far_clip_plane", 1000.0f},
                {"Ortho_width", 800.0f},
                {"Ortho_height", 600.0f},
                {"Use_perspective_matrix", true},

            }},
           {"Skybox", {
                {"Use_cubemap", false},
                {"DDS_file_path", "../stages/assets/textures/skybox/sky.dds"},
                {"DDS_vertex_file", "../stages/shaders/skybox/skybox_dds.vs"},
                {"DDS_fragment_file", "../stages/shaders/skybox/skybox_dds.fs"},
                {"DDS_angle", -90.0f},
                {"Cubemap_vertex_file", "../stages/shaders/skybox/skybox_cubemap.vs"},
                {"Cubemap_fragment_file", "../stages/shaders/skybox/skybox_cubemap.fs"},
                {"Cubemap_file_paths", {
                    "../stages/assets/textures/skybox/cubemap/nebulae/right.png",
                    "../stages/assets/textures/skybox/cubemap/nebulae/left.png",
                    "../stages/assets/textures/skybox/cubemap/nebulae/top.png",
                    "../stages/assets/textures/skybox/cubemap/nebulae/bottom.png",
                    "../stages/assets/textures/skybox/cubemap/nebulae/front.png",
                    "../stages/assets/textures/skybox/cubemap/nebulae/back.png"
                }}
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

    return filename;
}

glm::vec3 SaveManager::parseVec3(const nlohmann::json& j)
{
    return glm::vec3(j[0], j[1], j[2]);
}

glm::quat SaveManager::parseQuat(const nlohmann::json& j)
{
    return glm::quat(j[3], j[0], j[1], j[2]);
}


