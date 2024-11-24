#ifndef RNC
#define RNC

#include <memory>

class Mesh;

struct RenderComponent {
    std::shared_ptr<Mesh> render_mesh;

    RenderComponent() = default;


    RenderComponent(const RenderComponent& other)
        : render_mesh(other.render_mesh ? std::make_shared<Mesh>(*other.render_mesh) : nullptr) {}


    RenderComponent& operator=(const RenderComponent& other) {
        if (this != &other) {
            render_mesh = other.render_mesh ? std::make_shared<Mesh>(*other.render_mesh) : nullptr;
        }
        return *this;
    }
};

#endif