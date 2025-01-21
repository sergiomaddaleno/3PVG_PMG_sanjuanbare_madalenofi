#ifndef RNC
#define RNC

#include <memory>
#include "../Geometry/triangle.h"
#include "../Render/texture.h"
#include "../Render/shader.h"
#include "../Render/mesh.h"
#include "../Geometry/terrain.h"

enum RenderType {
    RT_Default,
    RT_Mesh,
    RT_Light,
    RT_Skybox,
    RT_PostProcess,
    RT_Transparent,
    RT_Terrain,
};

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    Triangle triangle_;
    std::shared_ptr<Terrain> terrain_;

    Shader shader;
    Shader skybox_shader;

    Texture texture;
    Texture skybox_tex;

    RenderType type_;
    //----

    RenderComponent() : mesh_(nullptr), terrain_(nullptr), triangle_(triangle_.my_triangle) , type_(RenderType::RT_Default) {}

    ~RenderComponent() {}

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr), terrain_(other.terrain_),
        triangle_(other.triangle_), texture(other.texture), skybox_tex(other.skybox_tex),
        shader(other.shader), skybox_shader(other.skybox_shader), type_(other.type_)
    {}

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;
            triangle_ = other.triangle_;
            terrain_ = other.terrain_;
            texture = other.texture;
            skybox_tex = other.skybox_tex;
            shader = other.shader;
            skybox_shader = other.skybox_shader;
            type_ = other.type_;
        }
        return *this;
    }
};

#endif
