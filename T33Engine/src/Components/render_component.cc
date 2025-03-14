#include "Components/render_component.h"

RenderComponent::RenderComponent()
    : mesh_(nullptr),
    terrain_(nullptr),
    sphere_(nullptr),
    cube_(nullptr),
    quad_(nullptr),
    surface_(nullptr),
    type_(RT_None),
    triangle_(triangle_.my_triangle)
{}

RenderComponent& RenderComponent::operator=(const RenderComponent & other)
{
    if (this != &other) {
        mesh_ = other.mesh_ ? std::make_unique<Mesh>(*other.mesh_) : nullptr;
        terrain_ = other.terrain_ ? std::make_unique<Terrain>(*other.terrain_) : nullptr;
        sphere_ = other.sphere_ ? std::make_unique<Sphere>(*other.sphere_) : nullptr;
        surface_ = other.surface_ ? std::make_unique<Surface>(*other.surface_) : nullptr;
        cube_ = other.cube_ ? std::make_unique<Cube>(*other.cube_) : nullptr;
        quad_ = other.quad_ ? std::make_unique<Quad>(*other.quad_) : nullptr;
    }
    return *this;
}

RenderComponent::RenderComponent(const RenderComponent& other)

    : mesh_(other.mesh_ ? std::make_unique<Mesh>(*other.mesh_) : nullptr),
    terrain_(other.terrain_ ? std::make_unique<Terrain>(*other.terrain_) : nullptr),
    sphere_(other.sphere_ ? std::make_unique<Sphere>(*other.sphere_) : nullptr),
    surface_(other.surface_ ? std::make_unique<Surface>(*other.surface_) : nullptr),
    cube_(other.cube_ ? std::make_unique<Cube>(*other.cube_) : nullptr),
    quad_(other.quad_ ? std::make_unique<Quad>(*other.quad_) : nullptr),
    triangle_(other.triangle_),
    type_(other.type_)
{}

nlohmann::json RenderComponent::serialize() const
{
    nlohmann::json json_data;

    if (mesh_) {
        json_data["Mesh"] = mesh_->serialize();
    }
    //json_data["triangle"] = { triangle_.ToJson() };

    if (terrain_) { json_data["Terrain"] = terrain_->serialize(); }
    if (sphere_) { json_data["Sphere"] = sphere_->serialize(); }
    if (surface_) { json_data["Surface"] = surface_->serialize(); }
    if (cube_) { json_data["Cube"] = cube_->serialize(); }
    if (quad_) { json_data["Quad"] = quad_->serialize(); }

    json_data["Render_type"] = (int)type_;

    return json_data;
}


