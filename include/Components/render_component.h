#ifndef RNC
#define RNC

#include <memory>
#include "../Geometry/triangle.h"
#include "../Render/texture.h"
#include "../Render/shader.h"
#include "../Render/mesh.h"
#include "../Geometry/terrain.h"
#include "../Geometry/sphere.h"
#include "../Geometry/cube.h"
#include "../Geometry/quad.h"

enum RenderType {
    RT_Default,
    RT_Mesh,
    RT_Light,
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
    std::unique_ptr<Terrain> terrain_;
    std::unique_ptr<Sphere> sphere_;
    std::unique_ptr<Cube> cube_;
    std::unique_ptr<Quad> quad_;
    std::unique_ptr<bool> light;

    Shader shader;
    Texture texture;

    RenderType type_;
    //----

    RenderComponent() : mesh_(nullptr), terrain_(nullptr), triangle_(triangle_.my_triangle) , type_(RenderType::RT_Default) {}

    ~RenderComponent() {}

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_unique<Mesh>(*other.mesh_) : nullptr),
        terrain_(other.terrain_ ? std::make_unique<Terrain>(*other.terrain_) : nullptr), 
        sphere_(other.sphere_ ? std::make_unique<Sphere>(*other.sphere_) : nullptr),
        cube_(other.cube_ ? std::make_unique<Cube>(*other.cube_) : nullptr),
        quad_(other.quad_ ? std::make_unique<Quad>(*other.quad_) : nullptr),
        light(other.light ? std::make_unique<bool>(*other.light) : nullptr),
        triangle_(other.triangle_),
        texture(other.texture),
        shader(other.shader),
        type_(other.type_)
    {}


    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_unique<Mesh>(*other.mesh_) : nullptr;
            terrain_ = other.terrain_ ? std::make_unique<Terrain>(*other.terrain_) : nullptr;
            sphere_ = other.sphere_ ? std::make_unique<Sphere>(*other.sphere_) : nullptr;
            cube_ = other.cube_ ? std::make_unique<Cube>(*other.cube_) : nullptr;
            quad_ = other.quad_ ? std::make_unique<Quad>(*other.quad_) : nullptr;
            light = other.light ? std::make_unique<bool>(*other.light) : nullptr;
        }
        return *this;
    }
};

#endif
