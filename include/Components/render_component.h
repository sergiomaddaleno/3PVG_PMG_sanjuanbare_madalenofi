#ifndef RNC
#define RNC

#include <memory>
#include "..\include\render.h"
#include "triangle.h"

class Mesh;

struct RenderComponent {

    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Triangle> triangle_;

    GLuint textureID;

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
