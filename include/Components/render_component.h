#ifndef RNC
#define RNC

#include <memory>
#include "../Geometry/triangle.h"
#include "../Core/texture.h"
#include "../Core/shader.h"
#include "../Core/mesh.h"
#include "Render/light.h"
#include "../Geometry/terrain.h"
#include "../Geometry/sphere.h"
#include "../Geometry/cube.h"
#include "../Geometry/quad.h"

#include "../deps/nlohmann/json.hpp"


enum RenderType {
    RT_Default,
    RT_Mesh,
    RT_PointLight,
    RT_SpotLight,
    RT_Skybox,
    RT_PostProcess,
    RT_Transparent,
    RT_Terrain,
    RT_Sphere,
    RT_Cube,
    RT_Quad,
};

struct RenderComponent {

    std::unique_ptr<Mesh> mesh_;

    Triangle triangle_;
    std::unique_ptr<Terrain>terrain_;
    std::unique_ptr<Sphere>sphere_;
    std::unique_ptr<Cube>cube_;
    std::unique_ptr<Quad>quad_;
    std::unique_ptr<Light>light_;

    Shader shader;
    Texture texture;
    RenderType type_;

    // Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        if (mesh_) {json_data["Mesh"] = mesh_->serialize();
        }
        //json_data["triangle"] = { triangle_.ToJson() };

        if (terrain_) { json_data["Terrain"] = terrain_->serialize(); }
        if (sphere_) { json_data["Sphere"] = sphere_->serialize(); }
        if (cube_) { json_data["Cube"] = cube_->serialize(); }
        if (quad_) { json_data["Quad"] = quad_->serialize(); }
        if (light_) { json_data["Light"] = light_->serialize(); }

        json_data["Shader"] = shader.serialize();
        json_data["Texture"] = texture.serialize();
        json_data["Render_type"] = (int)type_;

        return json_data;
    }

    // ----------------------------------

    RenderComponent()
        : mesh_(nullptr),
        terrain_(nullptr), 
        sphere_(nullptr), 
        cube_(nullptr),
        quad_(nullptr), 
        light_(nullptr),
        shader(),
        texture(), 
        triangle_(triangle_.my_triangle),
        type_(RenderType::RT_Default)
    {}
    ~RenderComponent() {}
    
    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_unique<Mesh>(*other.mesh_) : nullptr;
            terrain_ = other.terrain_ ? std::make_unique<Terrain>(*other.terrain_) : nullptr;
            sphere_ = other.sphere_ ? std::make_unique<Sphere>(*other.sphere_) : nullptr;
            cube_ = other.cube_ ? std::make_unique<Cube>(*other.cube_) : nullptr;
            quad_ = other.quad_ ? std::make_unique<Quad>(*other.quad_) : nullptr;
            light_ = other.light_ ? std::make_unique<Light>(*other.light_) : nullptr;
        }
        return *this;
    }

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_unique<Mesh>(*other.mesh_) : nullptr),
        terrain_(other.terrain_ ? std::make_unique<Terrain>(*other.terrain_) : nullptr),
        sphere_(other.sphere_ ? std::make_unique<Sphere>(*other.sphere_) : nullptr),
        cube_(other.cube_ ? std::make_unique<Cube>(*other.cube_) : nullptr),
        quad_(other.quad_ ? std::make_unique<Quad>(*other.quad_) : nullptr),
        light_(other.light_ ? std::make_unique<Light>(*other.light_) : nullptr),
        triangle_(other.triangle_),
        texture(other.texture),
        shader(other.shader),
        type_(other.type_)
    {}
};

#endif
