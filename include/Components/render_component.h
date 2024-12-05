#ifndef RNC
#define RNC

#include <memory>
#include "..\include\render.h"
#include "triangle.h"

class Mesh;

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Triangle> triangle_;

    GLuint program_ = 0;
    GLuint skybox_program_ = 0;

    GLuint textureID = -1;
    GLuint cubemap_tex = -1;

    RenderComponent() = default;

    RenderComponent(const RenderComponent& other)
        : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr),
        triangle_(other.triangle_)
    {}

    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;
           
            triangle_ = other.triangle_;
        }
        return *this;
    }

    ~RenderComponent() {
        
    }
};

#endif
