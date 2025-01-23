#ifndef RNC
#define RNC

#include <memory>
#include "../Geometry/triangle.h"
#include "../Render/texture.h"
#include "../Render/shader.h"
#include "../Render/mesh.h"
#include "../Geometry/terrain.h"
#include "../Geometry/sphere.h"

enum RenderType {
    RT_Default,
    RT_Mesh,
    RT_Light,
    RT_Skybox,
    RT_PostProcess,
    RT_Transparent,
    RT_Terrain,
    RT_Sphere,
};

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    Triangle triangle_;
    std::shared_ptr<Terrain> terrain_;
    std::shared_ptr<Sphere> sphere_;

    Shader shader;
    Texture texture;

    RenderType type_;
    //----

    RenderComponent() : mesh_(nullptr), terrain_(nullptr), triangle_(triangle_.my_triangle) , type_(RenderType::RT_Default) {}

    ~RenderComponent() {}

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr), terrain_(other.terrain_),
        triangle_(other.triangle_), texture(other.texture),
        shader(other.shader), type_(other.type_)
    {}

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;
            triangle_ = other.triangle_;
            terrain_ = other.terrain_;
            texture = other.texture;
            shader = other.shader;
            type_ = other.type_;
        }
        return *this;
    }
};

#endif
