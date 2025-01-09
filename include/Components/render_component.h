#ifndef RNC
#define RNC

#include <memory>
#include "..\include\render.h"
#include "triangle.h"
#include "texture.h"

class Mesh;

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Triangle> triangle_;

    GLuint program_ = 0;
    GLuint skybox_program_ = 0;

    Texture texture;
    Texture cubemap_tex;

    RenderComponent() : mesh_(nullptr), triangle_(nullptr), program_(0), skybox_program_(0) {}

    ~RenderComponent() {}

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr),
        triangle_(other.triangle_), texture(other.texture), cubemap_tex(other.cubemap_tex) {}

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;
            triangle_ = other.triangle_;
            texture = other.texture;
            cubemap_tex = other.cubemap_tex;
        }
        return *this;
    }
};

#endif
