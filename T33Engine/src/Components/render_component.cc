#include "Components/render_component.h"

RenderComponent::RenderComponent()
    : mesh_(nullptr),
    terrain_(nullptr),
    sphere_(nullptr),
    cube_(nullptr),
    quad_(nullptr),
    surface_(nullptr),
    water_(nullptr),
    cloud_(nullptr),
    type_(RT_None),
    triangle_(triangle_.my_triangle)
{}

RenderComponent& RenderComponent::operator=(const RenderComponent & other)
{
    if (this != &other) {
        mesh_ = other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr;
        terrain_ = other.terrain_ ? std::make_shared<Terrain>(*other.terrain_) : nullptr;
        water_ = other.water_ ? std::make_shared<Terrain>(*other.water_) : nullptr;
        sphere_ = other.sphere_ ? std::make_shared<Sphere>(*other.sphere_) : nullptr;
        surface_ = other.surface_ ? std::make_shared<Surface>(*other.surface_) : nullptr;
        cube_ = other.cube_ ? std::make_shared<Cube>(*other.cube_) : nullptr;
        quad_ = other.quad_ ? std::make_shared<Quad>(*other.quad_) : nullptr;
        cloud_ = other.cloud_ ? std::make_shared<RayMarch>(*other.cloud_) : nullptr;
    }
    return *this;
}

RenderComponent::RenderComponent(const RenderComponent& other)

    : mesh_(other.mesh_ ? std::make_shared<Mesh>(*other.mesh_) : nullptr),
    terrain_(other.terrain_ ? std::make_shared<Terrain>(*other.terrain_) : nullptr),
    water_(other.water_ ? std::make_shared<Terrain>(*other.water_) : nullptr),
    sphere_(other.sphere_ ? std::make_shared<Sphere>(*other.sphere_) : nullptr),
    surface_(other.surface_ ? std::make_shared<Surface>(*other.surface_) : nullptr),
    cube_(other.cube_ ? std::make_shared<Cube>(*other.cube_) : nullptr),
    quad_(other.quad_ ? std::make_shared<Quad>(*other.quad_) : nullptr),
    cloud_(other.cloud_ ? std::make_shared<RayMarch>(*other.cloud_) : nullptr),
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
    if (water_) { json_data["Water"] = water_->serialize(); }
    if (sphere_) { json_data["Sphere"] = sphere_->serialize(); }
    if (surface_) { json_data["Surface"] = surface_->serialize(); }
    if (cube_) { json_data["Cube"] = cube_->serialize(); }
    if (quad_) { json_data["Quad"] = quad_->serialize(); }
    if (cloud_) { json_data["Cloud"] = cloud_->serialize(); }

    json_data["Render_type"] = (int)type_;

    return json_data;
}


