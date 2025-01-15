#ifndef RNC
#define RNC

#include <memory>
#include "triangle.h"
#include "texture.h"
#include "shader.h"
#include "mesh.h"

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    Triangle triangle_;

    Shader shader;
    Shader skybox_shader;

    Texture texture;
    Texture skybox_tex;

    RenderComponent() : mesh_(nullptr), triangle_(triangle_.my_triangle) {}

    ~RenderComponent() {}

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr),
        triangle_(other.triangle_), texture(other.texture), skybox_tex(other.skybox_tex),
        shader(other.shader), skybox_shader(other.skybox_shader)
    {}

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;
            triangle_ = other.triangle_;
            texture = other.texture;
            skybox_tex = other.skybox_tex;
            shader = other.shader;
            skybox_shader = other.skybox_shader;
        }
        return *this;
    }
};

#endif
