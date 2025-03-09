

#include "Render/render_system.h"
#include "Core/mesh.h"
#include "Core/camera.h"
#include "Render/shadow_map.h"
#include "Core/shader.h"
#include "utils.h"
#include "global_data.h"
#include "Components/components.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <memory> 
#include <iostream>


RenderSystem::RenderSystem(ECSManager& ecs_)
    : shader(),
    entity_(0),
    entity_mesh(0),
    entity_terrain(0),
    entity_sphere(0),
    entity_cube8v(0),
    entity_cube24v(0),
    entity_quad(0),
    entity_spotlight(0),
    entity_pointlight(0),
    entity_dirlight(0),
    ecs_(ecs_)
{
}

RenderSystem::~RenderSystem() {}

void RenderSystem::init() {
    
    auto ri = ecs_.getComponentList<RenderComponent>().begin();
    auto li = ecs_.getComponentList<LightComponent>().begin();

    auto riend = ecs_.getComponentList<RenderComponent>().end();
    auto liend = ecs_.getComponentList<LightComponent>().end();

    for (; ri != riend; ++ri) {
        if (ri->has_value()) {
            auto& render_comp = *ri;

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
                render_comp->mesh_->bind();
                break;

            case RenderType::RT_Terrain:
                render_comp->terrain_->bind();
                render_comp->terrain_->setTexture(render_comp->terrain_->texture_path_);
                break;

            case RenderType::RT_Sphere:
                render_comp->sphere_->bind();
                render_comp->sphere_->setTexture(render_comp->sphere_->texture_path_);
                break;

            case RenderType::RT_Quad:
                render_comp->quad_->bind();
                render_comp->quad_->setTexture(render_comp->quad_->texture_path_);
                break;

            case RenderType::RT_Cube8v:
            case RenderType::RT_Cube24v:
                render_comp->cube_->bind();
                render_comp->cube_->setTexture(render_comp->cube_->texture_path_);
                break;
            }
            
            for (; li != liend; ++li) {
                if (li->has_value()) {
                    auto& light_comp = *li;

                    switch (light_comp->light.type_) {
                    case Light::LightType::LT_Spot:
                    case Light::LightType::LT_Directional:
                        light_comp->shadow_map_.init(); break;

                    case Light::LightType::LT_Point:
                        light_comp->shadow_map_.initCubeMap(); break;
                    }
                }
            }
        }
    }
    
    deferred.init();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void RenderSystem::setUniforms(TransformComponent tc, Shader shader, LightComponent light, Camera cam)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = TextureID::TI_Albedo;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), ValueType::T_INT_1, &tex_id);

    tex_id = TextureID::TI_Normal;
    shader.setUniformValue(shader.getUniformLocation("u_normal_map"), ValueType::T_INT_1, &tex_id);

    tex_id = TextureID::TI_Parallax;
    shader.setUniformValue(shader.getUniformLocation("u_parallax_map"), ValueType::T_INT_1, &tex_id);

    shader.setUniformValue(shader.getUniformLocation("u_fog"), ValueType::T_INT_1, &global.use_fog);
    shader.setUniformValue(shader.getUniformLocation("u_density"), ValueType::T_FLOAT_1, &global.fog_density);
    shader.setUniformValue(shader.getUniformLocation("u_gradient"), ValueType::T_FLOAT_1, &global.fog_gradient);
    shader.setUniformValue(shader.getUniformLocation("u_sky_colour"), ValueType::T_FLOAT_3, &global.sky_colour);
}

void RenderSystem::forwardRender(Camera cam) {
    
    glEnable(GL_DEPTH_TEST);
    glClear( GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    auto li = ecs_.getComponentList<LightComponent>().begin();
    auto liend = ecs_.getComponentList<LightComponent>().end();

    shader.useProgram();

    for (; li != liend; ++li) {
        if (li->has_value()) {
            auto& light_comp = *li;

            light_comp->shadow_map_.render(ecs_, shader, *light_comp);

            shader.setUniformValue(shader.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));
            
            light_comp->light.setUniforms(shader);
            
            // ---------------------------------------------------------------

            auto ri = ecs_.getComponentList<RenderComponent>().begin();
            auto ti = ecs_.getComponentList<TransformComponent>().begin();
            auto riend = ecs_.getComponentList<RenderComponent>().end();
            auto tiend = ecs_.getComponentList<TransformComponent>().end();

            for (; ri != riend && ti != tiend; ++ri, ++ti) {
                if (ri->has_value() && ti->has_value()) {
                    auto& render_comp = *ri;
                    auto& transform_comp = *ti;

                    setUniforms(*transform_comp, shader, *light_comp, cam);

                    switch (render_comp->type_) {
                    case RenderType::RT_Mesh:
                    {
                        for (auto& mesh : render_comp->mesh_->meshes) {
                            render_comp->mesh_->draw();
                        }
                    }
                    break;

                    case RenderType::RT_Terrain:
                    {
                        render_comp->terrain_->draw();

                        render_comp->terrain_->wave_.setUniforms(shader, (float)glfwGetTime());
                    }
                    break;

                    case RenderType::RT_Sphere:
                    {
                        render_comp->sphere_->draw();

                        //render_comp->sphere_->wave_.setUniforms(shader);
                    }
                    break;

                    case RenderType::RT_Cube8v:
                    case RenderType::RT_Cube24v:
                    {
                        render_comp->cube_->draw();

                        //render_comp->cube_->wave_.setUniforms(shader);
                    }
                    break;

                    case RenderType::RT_Quad:
                    {
                        render_comp->quad_->draw();

                        int use = render_comp->quad_->use_normal_map_;
                        shader.setUniformValue(shader.getUniformLocation("u_use_normal_map"), 
                            ValueType::T_INT_1, &use);

                        float scale = render_comp->quad_->parallax_scale_;
                        shader.setUniformValue(shader.getUniformLocation("u_parallax_scale"),
                            ValueType::T_FLOAT_1, &scale);
                        
                        //render_comp->quad_->wave_.setUniforms(shader);
                    }
                    break;
                    }
                }
            }
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }
    
    glDisable(GL_BLEND);
    glUseProgram(0);
}

void RenderSystem::deferredRender(Camera cam)
{
    deferred.resize(global.window_width, global.window_height);
    deferred.render(ecs_, cam);
}

void RenderSystem::setup()
{
    entity_ = ecs_.create_entity();

    ecs_.add_component_type<IdentifierComponent>();
    ecs_.add_component_type<RenderComponent>();
    ecs_.add_component_type<TransformComponent>();
    ecs_.add_component_type<LightComponent>();
}

size_t RenderSystem::createMesh(std::string file, std::string textureFolder, 
    TransformComponent tc)
{
    auto mesh = std::make_unique<Mesh>();
    if (!mesh->loadOBJ(file, textureFolder)) {
        return -1;
    }
    
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    //---

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Mesh [" + std::to_string(entity_mesh) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Mesh;
    ecs_.get_component<RenderComponent>(entity_)->mesh_ = std::move(mesh);

    entity_++;
    entity_mesh++;

    return entity_;
}

size_t RenderSystem::createTerrain(TransformComponent tc,
    const int num_cols, const int num_rows,
    const float height_mult, const float size, 
    const bool is_centered, const bool use_perlin_noise){

    auto terrain = std::make_unique<Terrain>();
    terrain->init(num_cols, num_rows, height_mult, size, is_centered, use_perlin_noise);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Terrain [" + std::to_string(entity_terrain) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Terrain;
    ecs_.get_component<RenderComponent>(entity_)->terrain_ = std::move(terrain);

    entity_++;
    entity_terrain++;

    return entity_;
}

size_t RenderSystem::createSphere(TransformComponent tc, int num_heights_, int num_revs_, float size)
{
    auto sphere = std::make_unique<Sphere>();
    sphere->init(size, num_heights_, num_revs_);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    //---

    ecs_.get_component<IdentifierComponent>(entity_)->name = "Sphere [" + std::to_string(entity_sphere) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Sphere;
    ecs_.get_component<RenderComponent>(entity_)->sphere_ = std::move(sphere);

    entity_++;
    entity_sphere++;

    return entity_;
}

size_t RenderSystem::createCube8v(TransformComponent tc, const float size, bool is_centered)
{
    auto cube = std::make_unique<Cube>();
    cube->init8v(size, is_centered);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Cube8v [" + std::to_string(entity_cube8v) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube8v;
    ecs_.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);

    entity_++;
    entity_cube8v++;

    return entity_;
}

size_t RenderSystem::createCube24v(TransformComponent tc, const float size)
{
    auto cube = std::make_unique<Cube>();
    cube->init24v(size);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Cube24v [" + std::to_string(entity_cube24v) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Cube24v;
    ecs_.get_component<RenderComponent>(entity_)->cube_ = std::move(cube);

    entity_++;
    entity_cube24v++;

    return entity_;
}

size_t RenderSystem::createQuad(TransformComponent tc, const float size)
{
    auto quad = std::make_unique<Quad>();
    quad->init(size);

    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<RenderComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Quad [" + std::to_string(entity_quad) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& transform = ecs_.get_component<TransformComponent>(entity_);
    transform->position = tc.position;
    transform->rotation = tc.rotation;
    transform->scale = tc.scale;

    ecs_.get_component<RenderComponent>(entity_)->type_ = RenderType::RT_Quad;
    ecs_.get_component<RenderComponent>(entity_)->quad_ = std::move(quad);

    entity_++;
    entity_quad++;

    return entity_;

}

size_t RenderSystem::createSpotLight(Light li, bool default_light)
{
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<LightComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Spot Light [" + std::to_string(entity_spotlight) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    ecs_.add_component<TransformComponent>(entity_)->position = li.pos_;

    // ----
    auto& light_comp = ecs_.get_component<LightComponent>(entity_);
    light_comp->light.type_ = Light::LightType::LT_Spot;

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
    entity_spotlight++;

    return entity_;
}

size_t RenderSystem::createPointLight(Light li, bool default_light)
{
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<TransformComponent>(entity_);
    ecs_.add_component<LightComponent>(entity_);

    //---
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Point Light [" + std::to_string(entity_spotlight) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    ecs_.get_component<TransformComponent>(entity_)->position = li.pos_;
    //--

    auto& light_comp = ecs_.get_component<LightComponent>(entity_);
    light_comp->light.type_ = Light::LightType::LT_Point;

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
    entity_pointlight++;

    return entity_;
}

size_t RenderSystem::createDirectionalLight(Light li, bool default_light)
{
    ecs_.add_component<IdentifierComponent>(entity_);
    ecs_.add_component<LightComponent>(entity_);

    // ----
    ecs_.get_component<IdentifierComponent>(entity_)->name = "Directional Light [" + std::to_string(entity_dirlight) + "]";
    ecs_.get_component<IdentifierComponent>(entity_)->id = (int)entity_;

    auto& light_comp = ecs_.get_component<LightComponent>(entity_);
    light_comp->light.type_ = Light::LightType::LT_Directional;
  
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
    entity_dirlight++;
    
    return entity_;
}

#define GLSL(x) "#version 330\n"#x
static const char* VertexVertexForBuild = GLSL(
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 u_camera_position;

out vec2 uv;
out vec3 normal;
out vec3 FragPos;

//Shadows
uniform mat4 u_light_space;
out vec4 FragPosLight;

uniform vec3 u_light_pos_;

//Normal mapping
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

// fog
out float visibility;
uniform float u_density = 0.06;
uniform float u_gradient = 2.0;

// wave effect
uniform float u_time;
uniform float u_wave_amplitude = 0.2;
uniform float u_wave_length = 2.0;
uniform float u_wave_speed = 1.0;
uniform vec2 u_wave_direction = vec2(1.0, 0.5);
uniform bool u_use_wave = false;

vec3 waveEffect(vec3 position) {
    if (!u_use_wave) return position;

    float k = 6.28318530718 / u_wave_length;
    float omega = k * u_wave_speed;
    vec2 dir = normalize(u_wave_direction);

    float wave = u_wave_amplitude * sin(k * dot(position.xz, dir) + omega * u_time);
    position.y += wave;

    return position;
}

void TBN(vec3 lightPos, vec3 viewPos) {
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    TangentLightPos = TBN * lightPos;
    TangentViewPos = TBN * viewPos;
    TangentFragPos = TBN * FragPos;
}

void main()
{
    vec3 local_pos = waveEffect(aPos);
    vec4 world_pos = model * vec4(local_pos, 1.0);

    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(local_pos, 1.0));
    FragPosLight = u_light_space * vec4(FragPos, 1.0);

    //Fog
    vec4 pos_relative_to_cam = view * world_pos;
    float distance = length(pos_relative_to_cam.xyz);
    visibility = exp(-pow((distance * u_density), u_gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    //TBN
    TBN(u_light_pos_, u_camera_position);

    gl_Position = projection * view * vec4(FragPos, 1.0);

}
);

static const char* FragmentVertexForBuild = GLSL(
    struct LightInfo {
    int type_;
    vec3 pos_;
    vec3 dir_;
    vec3 diff_color_;
    vec3 spec_color_;
    vec3 light_color_;
    vec3 ambient_color_;

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle_;
    float spot_softness_;
};

in vec3 normal;
in vec3 pos;
in vec2 uv;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 u_camera_position;

//Samplers
uniform sampler2D u_texture;
uniform sampler2D u_normal_map;
uniform sampler2D u_shadow_map;
uniform samplerCube u_depth_map;

uniform bool u_use_normal_map;

//Light
uniform LightInfo u_light;
uniform vec3 u_ambient;
uniform float u_far_plane;
in vec4 FragPosLight;

//TBN
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

//Fog
uniform bool u_fog;
uniform vec3 u_sky_colour;
in float visibility;

// --- Lights
vec3 PointLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float diffuseStrength = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation;

    vec3 viewDir = normalize(u_camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);

    vec3 specular = specStrength * lc.light_color_ * attenuation;

    return (diffuse + specular);
}

vec3 SpotLight(LightInfo lc, vec3 normal) {
    normal = normalize(normal);
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float cutOff = cos(radians(lc.spot_angle_));
    float outerCutOff = cos(radians(lc.spot_angle_ + lc.spot_softness_));
    float theta = dot(lightDir, normalize(lc.dir_));

    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float intensity = smoothstep(outerCutOff, cutOff, theta);

    float diffuseStrength = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation * intensity;

    vec3 viewDir = normalize(u_camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);
    vec3 specular = lc.spec_color_ * specStrength * attenuation * intensity;

    vec3 ambient = lc.ambient_color_ * u_ambient * intensity;

    return ambient + diffuse + specular;
}

vec3 DirectionalLight(LightInfo lc) {

    vec3 light_dir = normalize(-lc.dir_);

    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * lc.light_color_ * lc.diff_color_ * lc.strength_;

    vec3 view_dir = normalize(u_camera_position - FragPos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), lc.shininess_);
    vec3 specular = spec * lc.spec_color_;

    vec3 ambient = lc.ambient_color_ * lc.light_color_;

    return ambient + diffuse + specular;
}

// --- Shadows

//percentage-closer filtering
float PCF(float currentDepth, float bias, vec3 projCoords) {
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y < 1; ++y) {

            float pcfDepth = texture(u_shadow_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(u_shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.0005;

    //return (currentDepth - bias) > closestDepth ? 0.0 : 1.0;
    return 1.0 - PCF(currentDepth, bias, projCoords);
}

float ShadowCalculationPoint(vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float closestDepth = texture(u_depth_map, fragToLight).r;
    closestDepth *= u_far_plane;

    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return 1.0 - shadow;
}

void main() {

    vec3 final_light = u_ambient;
    float shadow = ShadowCalculation(FragPosLight);
    float shadow_pl = ShadowCalculationPoint(FragPosLight.xyz, u_light.pos_);

    vec3 mapped_normal = texture(u_normal_map, uv).rgb;
    mapped_normal = normalize(mapped_normal * 2.0 - 1.0);

    if (u_light.type_ == 0) {
        final_light += DirectionalLight(u_light) * shadow;
    }

    if (u_light.type_ == 1) {
        vec3 my_normal;

        if (u_use_normal_map) {
            mat3 TBN = mat3(TangentLightPos, TangentViewPos, TangentFragPos);
            my_normal = normalize(TBN * mapped_normal);
        }
        else {
            my_normal = normal;
        }

        final_light += SpotLight(u_light, my_normal) * shadow;
    }

    if (u_light.type_ == 2) {
        final_light += PointLight(u_light) * shadow;
    }

    vec3 texture_color = texture(u_texture, uv).rgb;

    vec3 final_color = texture_color * final_light;
    FragColor = vec4(final_color, 1.0);

    if (u_fog) FragColor = mix(vec4(u_sky_colour, 1.0), FragColor, visibility);
}
);

void RenderSystem::compileShaderForBuild()
{
    shader.compile(VertexVertexForBuild, FragmentVertexForBuild);
}
