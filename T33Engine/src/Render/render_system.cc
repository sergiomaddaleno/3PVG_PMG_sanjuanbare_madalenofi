

#include "Render/render_system.h"
#include "Core/mesh.h"
#include "Core/camera.h"
#include "Render/shadow_map.h"
#include "Core/shader.h"
#include "utils.h"
#include "Components/components.h"

#include "Managers/resource_manager.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"
#include "Geometry/surface.h"

#include "Render/raymarch.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <memory> 
#include <iostream>
#include <random>

RenderSystem::RenderSystem(ECSManager& ecs_)
    : shader(),
    entity_(0),
    ecs_(ecs_),
    window_width(800),
    window_height(600),
    alpha(1.0f),
    alpha_blend(0.8f),
    sky_colour(0.5f, 0.5f, 0.5f),
    use_fog(false),
    fog_density(0.1f),
    fog_gradient(1.0f),
    use_skybox(true),
    is_playing(false),
    light_in_scene(false),
    deferred(ecs_),
    ssao(ecs_),
    what_to_render(WTR_forward)
{
}

RenderSystem::~RenderSystem() {}

void RenderSystem::initRenderTypes() {

    auto ri = ecs_.getComponentList<RenderComponent>().begin();
    auto riend = ecs_.getComponentList<RenderComponent>().end();

    for (; ri != riend; ++ri) {
        if (ri->has_value()) {
            auto& render_comp = *ri;

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
                render_comp->mesh_->bind();
                break;
            case RenderType::RT_Terrain:
                render_comp->terrain_->bind();
                break;
            case RenderType::RT_Water:
                render_comp->water_->bind();
                break;
            case RenderType::RT_Sphere:
                render_comp->sphere_->bind();
                break;
            case RenderType::RT_Quad:
                render_comp->quad_->bind();
                break;
            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v:
                render_comp->cube_->bind();
                break;
            case RenderType::RT_Surface:
                render_comp->surface_->bind();
                break;
            case RenderType::RT_Cloud:
                render_comp->cloud_->cube.bind();
                break;
            }
        }
    }
}

void RenderSystem::initLights() {
    
    auto li = ecs_.getComponentList<LightComponent>().begin();
    auto liend = ecs_.getComponentList<LightComponent>().end();

    for (; li != liend; ++li) {
        if (li->has_value()) {
            auto& light_comp = *li;

            switch (light_comp->light.type_) {
            case Light::LightType::LT_Spot:
            case Light::LightType::LT_Directional:
                light_comp->shadow_map_->init2D();
                break;
            case Light::LightType::LT_Point:
                light_comp->shadow_map_->initCubeMap();
                break;
            }
        }
    }
}

void RenderSystem::initSkybox() {

    auto si = ecs_.getComponentList<SkyboxComponent>().begin();
    auto siend = ecs_.getComponentList<SkyboxComponent>().end();

    for (; si != siend; ++si) {
        if (si->has_value()) {
            auto& skybox_comp = *si;

            if (skybox_comp->skybox.use_cubemap) {
                skybox_comp->skybox.loadCubeMap(
                    skybox_comp->skybox.faces_files_,
                    skybox_comp->skybox.cubemap_vertex_file_.c_str(),
                    skybox_comp->skybox.cubemap_fragment_file_.c_str()
                );
                skybox_comp->skybox.initcubeMap();
            }
            else {
                skybox_comp->skybox.loadDDS(skybox_comp->skybox.dds_file_.c_str(),
                    skybox_comp->skybox.dds_vertex_file_.c_str(),
                    skybox_comp->skybox.dds_fragment_file_.c_str()
                );
                skybox_comp->skybox.initDDS();
            }
        }
    }
}

void RenderSystem::setUniforms(int entity, TransformComponent& tc, Shader& shader, Camera& cam)
{
    glm::mat4 model = getWorldTransform(entity);
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = TextureID::TI_Diffuse;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), ValueType::T_INT_1, &tex_id);

    tex_id = TextureID::TI_Normal;
    shader.setUniformValue(shader.getUniformLocation("u_normal_map"), ValueType::T_INT_1, &tex_id);

    tex_id = TextureID::TI_Parallax;
    shader.setUniformValue(shader.getUniformLocation("u_parallax_map"), ValueType::T_INT_1, &tex_id);

    shader.setUniformValue(shader.getUniformLocation("u_alpha"), ValueType::T_FLOAT_1, &alpha);

    int loc_use_fog = use_fog;
    shader.setUniformValue(shader.getUniformLocation("u_fog"), ValueType::T_INT_1, &loc_use_fog);
    shader.setUniformValue(shader.getUniformLocation("u_density"), ValueType::T_FLOAT_1, &fog_density);
    shader.setUniformValue(shader.getUniformLocation("u_gradient"), ValueType::T_FLOAT_1, &fog_gradient);
    shader.setUniformValue(shader.getUniformLocation("u_sky_colour"), ValueType::T_FLOAT_3, &sky_colour);
}

void RenderSystem::forwardRender(Input &input, double dt) {

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ZERO);
    //glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();

    shader.useProgram();

    for (; ci != ciend; ++ci) {
        if (ci->has_value()) {
            auto& camera_comp = *ci;

            if (window_width != camera_comp->window_width ||
                window_height != camera_comp->window_height) {
                camera_comp->window_width = window_width;
                camera_comp->window_height = window_height;
                camera_comp->camera.setAspectRatio((float)camera_comp->window_width / (float)camera_comp->window_height);
            }
            camera_comp->camera.update(input, dt, camera_comp->window_width, camera_comp->window_height);

            auto si = ecs_.getComponentList<SkyboxComponent>().begin();
            auto siend = ecs_.getComponentList<SkyboxComponent>().end();
            
            for (; si != siend; ++si) {
                if (si->has_value()) {
                    auto& skybox_comp = *si;
                    if (use_skybox) skybox_comp->skybox.render(camera_comp->camera);
                }
            }

            if (!is_playing) grid.drawGrid(camera_comp->camera);

            light_in_scene = false;

            auto li = ecs_.getComponentList<LightComponent>().begin();
            auto liend = ecs_.getComponentList<LightComponent>().end();
            
            for (; li != liend; ++li) {
                if (li->has_value()) {
                    auto& light_comp = *li;

                    light_in_scene = true;

                    light_comp->shadow_map_->resize(window_width, window_height);
                    light_comp->shadow_map_->render(shader, *light_comp);

                    shader.setUniformValue(shader.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(camera_comp->camera.getPosition()));
                    
                    light_comp->light.setUniforms(shader);

                    // ---------------------------------------------------------------

                    auto ri = ecs_.getComponentList<RenderComponent>().begin();
                    auto ti = ecs_.getComponentList<TransformComponent>().begin();
                    auto riend = ecs_.getComponentList<RenderComponent>().end();
                    auto tiend = ecs_.getComponentList<TransformComponent>().end();

                    int entity = 0;
                    for (; ri != riend && ti != tiend; ++ri, ++ti, ++entity) {
                        if (ri->has_value() && ti->has_value()) {
                            auto& render_comp = *ri;
                            auto& transform_comp = *ti;

                            this->setUniforms(entity, *transform_comp, shader, camera_comp->camera);
                            this->renderScene(*render_comp, shader);

                            /* Test for wave
                            static float a = 0;
                            static float w = 0;
                            static float s = 0;
                            static glm::vec2 d = {0.0f, 0.0f};

                            if (render_comp->type_ == RT_Water) {
                                a = render_comp->water_->wave_.amplitude_;
                                w = render_comp->water_->wave_.wave_length_;
                                s = render_comp->water_->wave_.speed_;
                                d = render_comp->water_->wave_.direction_;
                            }
                            if (render_comp->type_ == RT_Mesh) {

                                render_comp->mesh_->wave_.amplitude_ = a;
                                render_comp->mesh_->wave_.wave_length_ = w;
                                render_comp->mesh_->wave_.speed_ = s;
                                render_comp->mesh_->wave_.direction_ = d;

                                transform_comp->position.y = render_comp->mesh_->wave_.waveEffect(transform_comp->position.x, transform_comp->position.z);
                            }*/
                        }
                    }
                    glBlendFunc(GL_ONE, GL_ONE);
                }
            }

            if (!light_in_scene) {

                texture_shader.useProgram();

                auto ri = ecs_.getComponentList<RenderComponent>().begin();
                auto ti = ecs_.getComponentList<TransformComponent>().begin();
                auto riend = ecs_.getComponentList<RenderComponent>().end();
                auto tiend = ecs_.getComponentList<TransformComponent>().end();

                int entity = 0;
                for (; ri != riend && ti != tiend; ++ri, ++ti, ++entity) {
                    if (ri->has_value() && ti->has_value()) {
                        auto& render_comp = *ri;
                        auto& transform_comp = *ti;

                        this->setUniforms(entity, *transform_comp, texture_shader, camera_comp->camera);
                        this->renderScene(*render_comp, texture_shader);
                    }
                }
            }
        }
    }

    glDisable(GL_BLEND);
    glUseProgram(0);
}

void RenderSystem::deferredRender(Input& input, double dt)
{
    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();

    for (; ci != ciend; ++ci) {
        if (ci->has_value()) {
            auto& camera_comp = *ci;

            if (window_width != camera_comp->window_width ||
                window_height != camera_comp->window_height) {
                camera_comp->window_width = window_width;
                camera_comp->window_height = window_height;
                camera_comp->camera.setAspectRatio((float)camera_comp->window_width / (float)camera_comp->window_height);
            }
            camera_comp->camera.update(input, dt, camera_comp->window_width, camera_comp->window_height);


            deferred.resize(window_width, window_height);
            deferred.render(camera_comp->camera);

            //--

            auto li = ecs_.getComponentList<LightComponent>().begin();
            auto liend = ecs_.getComponentList<LightComponent>().end();

            for (; li != liend; ++li) {
                if (li->has_value()) {
                    auto& light_comp = *li;
                    light_comp->shadow_map_->resize(window_width, window_height);
                }
            }

            /*auto si = ecs_.getComponentList<SkyboxComponent>().begin();
            auto siend = ecs_.getComponentList<SkyboxComponent>().end();

            for (; si != siend; ++si) {
                if (si->has_value()) {
                    auto& skybox_comp = *si;

                    if (use_skybox) skybox_comp->skybox.render(camera_comp->camera);
                }
            }*/

            if (!is_playing) grid.drawGrid(camera_comp->camera);
        }
    }
}

void RenderSystem::SSAORender(Input& input, double dt)
{
    auto ci = ecs_.getComponentList<CameraComponent>().begin();
    auto ciend = ecs_.getComponentList<CameraComponent>().end();

    for (; ci != ciend; ++ci) {
        if (ci->has_value()) {
            auto& camera_comp = *ci;

            if (window_width != camera_comp->window_width ||
                window_height != camera_comp->window_height) {
                camera_comp->window_width = window_width;
                camera_comp->window_height = window_height;
                camera_comp->camera.setAspectRatio((float)camera_comp->window_width / (float)camera_comp->window_height);
            }
            camera_comp->camera.update(input, dt, camera_comp->window_width, camera_comp->window_height);

            ssao.resize(window_width, window_height);
            ssao.render(camera_comp->camera);

            //--

            auto li = ecs_.getComponentList<LightComponent>().begin();
            auto liend = ecs_.getComponentList<LightComponent>().end();

            for (; li != liend; ++li) {
                if (li->has_value()) {
                    auto& light_comp = *li;
                    light_comp->shadow_map_->resize(window_width, window_height);
                }
            }

            if (!is_playing) grid.drawGrid(camera_comp->camera);
        }
    }

}

void RenderSystem::renderScene(RenderComponent& rc, Shader& shader)
{
    switch (rc.type_) {
    case RenderType::RT_Mesh:
    {
        for (auto& mesh : rc.mesh_->meshes) {
            rc.mesh_->wave_.setUniforms(shader, (float)glfwGetTime());
            rc.mesh_->draw();
        }
    }
    break;
    case RenderType::RT_Terrain:
    {
        rc.terrain_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.terrain_->draw();
    }
    break;
    case RenderType::RT_Sphere:
    {
        rc.sphere_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.sphere_->draw();
    }
    break;
    case RenderType::RT_Surface:
    {
        rc.surface_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.surface_->draw();
    }
    break;
    case RenderType::RT_Cube8v:
    case RenderType::RT_Cube24v:
    {
        rc.cube_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.cube_->draw();
    }
    break;
    case RenderType::RT_Quad:
    {
        rc.quad_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.quad_->draw();

        int use = rc.quad_->use_normal_map_;
        shader.setUniformValue(shader.getUniformLocation("u_use_normal_map"),
            ValueType::T_INT_1, &use);

        use = rc.quad_->use_parallax_map_;
        shader.setUniformValue(shader.getUniformLocation("u_use_parallax_map"),
            ValueType::T_INT_1, &use);

        float scale = rc.quad_->parallax_scale_;
        shader.setUniformValue(shader.getUniformLocation("u_parallax_scale"),
            ValueType::T_FLOAT_1, &scale);

        int type = rc.quad_->parallax_type;
        shader.setUniformValue(shader.getUniformLocation("u_parallax_type"),
            ValueType::T_INT_1, &type);
    }
    break;
    case RenderType::RT_Cloud:
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        rc.cloud_->resize(window_width, window_height);
        rc.cloud_->render((float)glfwGetTime());

        glBlendFunc(GL_ONE, GL_ZERO);
    }
    break;
    case RenderType::RT_Water:
        // For Blend
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shader.setUniformValue(shader.getUniformLocation("u_alpha"), ValueType::T_FLOAT_1, &alpha_blend);

        rc.water_->wave_.setUniforms(shader, (float)glfwGetTime());
        rc.water_->draw();

        glBlendFunc(GL_ONE, GL_ZERO);
    break;
    }

    shader.useProgram();
}

glm::mat4 RenderSystem::getWorldTransform(int entity)
{
    if (!ecs_.has_component<TransformComponent>(entity))
        return glm::mat4(1.0f);

    auto& transform = ecs_.get_component<TransformComponent>(entity);
    glm::mat4 local = glm::translate(glm::mat4(1.0f), transform->position) *
        glm::mat4_cast(transform->rotation) *
        glm::scale(glm::mat4(1.0f), transform->scale);

    if (ecs_.has_component<HierarchyComponent>(entity)) {
        auto& hierarchy = ecs_.get_component<HierarchyComponent>(entity);
        if (hierarchy->hasParent()) {
            int parent = hierarchy->getParent();
            return getWorldTransform(parent) * local;
        }
    }

    return local;
}

void RenderSystem::spawnEntitiesInTerrain(int terrain_id, RenderType spawn_type, float spawn_prob, float y_spawn_height)
{
    std::vector<glm::vec3> spawn_positions;

    auto& render_comp = ecs_.get_component<RenderComponent>(terrain_id);
    auto& transf_comp = ecs_.get_component<TransformComponent>(terrain_id);

    if (!render_comp || !transf_comp) {
        AddConsoleMessage("Terrain ID not valid");
        return;
    }

    if (render_comp->type_ == RenderType::RT_Terrain) {
        std::vector<float> model_vertices = render_comp->terrain_->GetVertexTerrain();

        glm::vec3 terrain_position = transf_comp->position;
        glm::quat terrain_rotation = transf_comp->rotation;
        glm::vec3 terrain_scale = transf_comp->scale;

        for (int i = 0; i < model_vertices.size() / 3; i++) {
            glm::vec3 vertex = {
                model_vertices.at(i * 3 + 0),
                model_vertices.at(i * 3 + 1),
                model_vertices.at(i * 3 + 2) 
            };

            glm::vec3 transf_vertex = terrain_rotation * 
                (vertex * terrain_scale);

            glm::vec3 final_pos = transf_vertex + terrain_position;

            if (vertex.y > y_spawn_height && (float)rand() / RAND_MAX < spawn_prob) {
                spawn_positions.push_back(final_pos);
            }
        }
    }

    for (const auto& position : spawn_positions) {
        size_t entity = (size_t)0;
        switch (spawn_type) {
        case RenderType::RT_Cube8v:
            entity = createCube8v();
            break;
        case RenderType::RT_Cube24v:
            entity = createCube24v();
            break;
        case RenderType::RT_Quad:
            entity = createQuad();
            break;
        case RenderType::RT_Sphere:
            entity = createSphere();
            break;
        }

        auto& transform = ecs_.get_component<TransformComponent>(entity - 1);
        if (transform) {
            transform->position = position;
        }
    }

    this->initRenderTypes();
    AddConsoleMessage("Spawned entities added to scene");
}

void RenderSystem::spawnMeshInTerrain(int terrain_id, std::string file, std::string textureFolder, float spawn_prob, float y_spawn_height)
{
    std::vector<glm::vec3> spawn_positions;

    auto& render_comp = ecs_.get_component<RenderComponent>(terrain_id);
    auto& transf_comp = ecs_.get_component<TransformComponent>(terrain_id);

    if (!render_comp || !transf_comp) {
        AddConsoleMessage("Terrain ID not valid");
        return;
    }

    if (render_comp->type_ == RenderType::RT_Terrain) {
        std::vector<float> model_vertices = render_comp->terrain_->GetVertexTerrain();

        glm::vec3 terrain_position = transf_comp->position;
        glm::quat terrain_rotation = transf_comp->rotation;
        glm::vec3 terrain_scale = transf_comp->scale;

        for (int i = 0; i < model_vertices.size() / 3; i++) {
            glm::vec3 vertex = {
                model_vertices.at(i * 3 + 0),
                model_vertices.at(i * 3 + 1),
                model_vertices.at(i * 3 + 2)
            };

            glm::vec3 transf_vertex = terrain_rotation * (vertex * terrain_scale);

            glm::vec3 final_pos = transf_vertex + terrain_position;

            if (vertex.y > y_spawn_height && (float)rand() / RAND_MAX < spawn_prob) {
                spawn_positions.push_back(final_pos);
            }
        }
    }

    for (const auto& position : spawn_positions) {
        size_t entity;
        entity = createMesh(file, textureFolder);

        auto& transform = ecs_.get_component<TransformComponent>(entity - 1);
        if (transform) {
            transform->position = position;
        }
    }

    this->initRenderTypes();
    AddConsoleMessage(file + " entities added to scene");
}

void RenderSystem::resize(unsigned int width, unsigned int height)
{
    if (window_width != width || window_height != height) {

        window_width = width;
        window_height = height;
    }
}

size_t RenderSystem::addCamera(Camera camera)
{
    ecs_.add_component<CameraComponent>(entity_);
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Camera [" + std::to_string(entity_counts_.camera) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    ecs_.get_component<CameraComponent>(entity_)->camera = camera;

    entity_++;
    entity_counts_.camera++;

    return entity_;
}

size_t RenderSystem::addSkybox(Skybox skybox)
{
    skybox.loadDDS("../stages/assets/textures/skybox/sky.dds", 
        "../stages/shaders/skybox/skybox_dds.vs",
        "../stages/shaders/skybox/skybox_dds.fs"
    );
    skybox.initDDS();

    ecs_.add_component<SkyboxComponent>(entity_);
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Skybox [" + std::to_string(entity_counts_.skybox) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    ecs_.get_component<SkyboxComponent>(entity_)->skybox = skybox;

    entity_++;
    entity_counts_.skybox++;

    return entity_;
}

size_t RenderSystem::createCloud(float size, TransformComponent tc)
{
    auto cloud = std::make_shared<RayMarch>(ecs_);

    cloud->initRaymarchShader();
    cloud->loadNoiseTexture2D();
    cloud->loadNoiseTexture3D();
    cloud->initBoundingBox(size);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Cloud [" + std::to_string(entity_counts_.cloud) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cloud;
    ecs_.get_component<RenderComponent>(entity_)->cloud_ = cloud;

    entity_++;

    entity_counts_.cloud++;

    return entity_;
}

size_t RenderSystem::createMesh(std::string file, std::string textureFolder, TransformComponent tc)
{
    std::shared_ptr<Mesh> mesh;

    auto file_path = ResourceManager::GetInstance().FindFilePath(file);
    auto texture_path = ResourceManager::GetInstance().FindFilePath(textureFolder);

    mesh = ResourceManager::GetInstance().FindMesh(file_path, texture_path);

    if (!mesh) {
        mesh = std::make_shared<Mesh>();

        if (!mesh->loadOBJ(*file_path, *texture_path)) {
            return -1;
        }

        ResourceManager::GetInstance().StoreMesh(file_path, texture_path, mesh);
    }
    else {
        mesh = std::make_shared<Mesh>(*mesh);
    }

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Mesh [" + std::to_string(entity_counts_.mesh) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Mesh;
    ecs_.get_component<RenderComponent>(entity_)->mesh_ = mesh;

    entity_++;
    entity_counts_.mesh++;

    return entity_;
}

size_t RenderSystem::createTerrain(TransformComponent tc,
    const int num_cols, const int num_rows,
    const float height_mult, const float size, 
    const bool is_centered, const bool use_perlin_noise){

    auto terrain = std::make_shared<Terrain>();

    terrain->setTexture("../stages/assets/textures/images/grass.png");
    terrain->init(num_cols, num_rows, height_mult, size, is_centered, use_perlin_noise);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Terrain [" + std::to_string(entity_counts_.terrain) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Terrain;
    ecs_.get_component<RenderComponent>(entity_)->terrain_ = terrain;

    entity_++;
    entity_counts_.terrain++;

    return entity_;
}

size_t RenderSystem::createWater(TransformComponent tc,
    const int num_cols, const int num_rows, const float size,
    const bool is_centered) {

    auto water = std::make_shared<Terrain>();

    water->setTexture("../stages/assets/textures/images/blue.png");
    water->init(num_cols, num_rows, 0.0f, size, is_centered, true);
    water->wave_.use_wave_ = true;

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Water [" + std::to_string(entity_counts_.water) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Water;
    ecs_.get_component<RenderComponent>(entity_)->water_ = water;

    entity_++;
    entity_counts_.water++;

    return entity_;
}

size_t RenderSystem::createSphere(TransformComponent tc, int num_heights_, int num_revs_, float size)
{
    auto sphere = std::make_shared<Sphere>();

    sphere->setTexture("../stages/assets/textures/images/blue.png");
    sphere->init(size, num_heights_, num_revs_);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Sphere [" + std::to_string(entity_counts_.sphere) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Sphere;
    ecs_.get_component<RenderComponent>(entity_)->sphere_ = sphere;

    entity_++;
    entity_counts_.sphere++;

    return entity_;
}

size_t RenderSystem::createSurface(std::vector<float> surface_points, TransformComponent tc, const int num_heights, const int num_revs)
{
    auto surface = std::make_shared<Surface>();

    surface->setTexture("../stages/assets/textures/images/blue.png");
    surface->init(surface_points, num_heights, num_revs);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Surface [" + std::to_string(entity_counts_.surface) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Surface;
    ecs_.get_component<RenderComponent>(entity_)->surface_ = std::move(surface);

    entity_++;
    entity_counts_.surface++;

    return entity_;
}

size_t RenderSystem::createCube8v(TransformComponent tc, const float size, bool is_centered)
{
    auto cube = std::make_shared<Cube>();

    cube->setTexture("../stages/assets/textures/images/blue.png");
    cube->init8v(size, is_centered);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Cube8v [" + std::to_string(entity_counts_.cube8v) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube8v;
    ecs_.get_component<RenderComponent>(entity_)->cube_ = cube;

    entity_++;
    entity_counts_.cube8v++;

    return entity_;
}

size_t RenderSystem::createCube24v(TransformComponent tc, const float size)
{
    auto cube = std::make_shared<Cube>();

    cube->setTexture("../stages/assets/textures/images/blue.png");
    cube->init24v(size);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Cube24v [" + std::to_string(entity_counts_.cube24v) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube24v;
    ecs_.get_component<RenderComponent>(entity_)->cube_ = cube;

    entity_++;
    entity_counts_.cube24v++;

    return entity_;
}

size_t RenderSystem::createQuad(TransformComponent tc, const float size)
{
    auto quad = std::make_shared<Quad>();

    quad->setTexture("../stages/assets/textures/images/blue.png");
    quad->init(size);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Quad [" + std::to_string(entity_counts_.quad) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Quad;
    ecs_.get_component<RenderComponent>(entity_)->quad_ = quad;

    entity_++;
    entity_counts_.quad++;

    return entity_;

}

size_t RenderSystem::createSpotLight(Light li, bool default_light)
{
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<LightComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Spot Light [" + std::to_string(entity_counts_.spotlight) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    ecs_.add_component<TransformComponent>(entity_)->position = li.pos_;

    // ----
    auto& light_comp = ecs_.get_component<LightComponent>(entity_);
    light_comp->light.type_ = Light::LightType::LT_Spot;
    light_comp->shadow_map_ = std::make_unique<ShadowMap>(ecs_);


    if (default_light) {

        light_comp->light.pos_ = glm::vec3(0.0f, 0.0f, 0.0f);
        light_comp->light.dir_ = glm::vec3(0.0f, -1.0f, -1.0f);
        light_comp->light.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
        light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.linear_att = 0.7f;
        light_comp->light.quad_att_ = 1.8f;
        light_comp->light.constant_att_ = 1.0f;
        light_comp->light.shininess_ = 32.0f;
        light_comp->light.strength_ = 1.0f;
        light_comp->light.spot_angle_ = 45.0f;
        light_comp->light.spot_softness_ = 10.0f;
    }
    else {
        light_comp->light.pos_ = li.pos_;
        light_comp->light.dir_ = li.dir_;
        light_comp->light.light_color_ = li.light_color_;
        light_comp->light.linear_att = li.linear_att;
        light_comp->light.quad_att_ = li.quad_att_;
        light_comp->light.constant_att_ = li.constant_att_;
        light_comp->light.shininess_ = li.shininess_;
        light_comp->light.strength_ = li.strength_;
        light_comp->light.spot_angle_ = li.spot_angle_;
        light_comp->light.spot_softness_ = li.spot_softness_;
    }

    entity_++;
    entity_counts_.spotlight++;

    return entity_;
}

size_t RenderSystem::createPointLight(Light li, bool default_light)
{
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<LightComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Point Light [" + std::to_string(entity_counts_.pointlight) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    ecs_.get_component<TransformComponent>(entity_)->position = li.pos_;
    //--

    auto& light_comp = ecs_.get_component<LightComponent>(entity_);
    light_comp->light.type_ = Light::LightType::LT_Point;
    light_comp->shadow_map_ = std::make_unique<ShadowMap>(ecs_);

    if (default_light) {
        light_comp->light.pos_ = li.pos_;
        light_comp->light.light_color_ = glm::vec3(150.0f, 150.0f, 150.0f);
        light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.linear_att = 0.7f;
        light_comp->light.quad_att_ = 1.8f;
        light_comp->light.constant_att_ = 1.0f;
        light_comp->light.shininess_ = 32.0f;
        light_comp->light.strength_ = 1.0f;
    }
    else {
        light_comp->light.pos_ = li.pos_;
        light_comp->light.light_color_ = li.light_color_;
        light_comp->light.ambient_color_ = li.ambient_color_;
        light_comp->light.linear_att = li.linear_att;
        light_comp->light.quad_att_ = li.quad_att_;
        light_comp->light.constant_att_ = li.constant_att_;
        light_comp->light.shininess_ = li.shininess_;
        light_comp->light.strength_ = li.strength_;
    }
    
    entity_++;
    entity_counts_.pointlight++;

    return entity_;
}

size_t RenderSystem::createDirectionalLight(Light li, bool default_light)
{
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<LightComponent>(entity_);
    ecs_.add_component<HierarchyComponent>(entity_);

    // ----
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Directional Light [" + std::to_string(entity_counts_.dirlight) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& light_comp = ecs_.get_component<LightComponent>(entity_);
    light_comp->light.type_ = Light::LightType::LT_Directional;
    light_comp->shadow_map_ = std::make_unique<ShadowMap>(ecs_);

    if (default_light) {
        light_comp->light.dir_ = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
        light_comp->light.light_color_ = glm::vec3(10.0f, 10.0f, 10.0f);
        light_comp->light.ambient_color_ = glm::vec3(0.1f, 0.1f, 0.1f);
        light_comp->light.spec_color_ = glm::vec3(1.0f, 1.0f, 0.95f);
        light_comp->light.linear_att = 0.0f;
        light_comp->light.quad_att_ = 0.0f;
        light_comp->light.constant_att_ = 1.0f;
        light_comp->light.shininess_ = 34.0f;
    }else{
        light_comp->light.dir_ = glm::normalize(li.dir_);
        light_comp->light.light_color_ = li.light_color_;
        light_comp->light.ambient_color_ = li.ambient_color_;
        light_comp->light.spec_color_ = li.spec_color_;
        light_comp->light.linear_att = li.linear_att;
        light_comp->light.quad_att_ = li.quad_att_;
        light_comp->light.constant_att_ = li.constant_att_;
        light_comp->light.shininess_ = li.shininess_;
    }

    entity_++;
    entity_counts_.dirlight++;
    
    return entity_;
}

