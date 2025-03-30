#include "Components/physics_component.h"

PhysicsComponent::PhysicsComponent() 
    : gravity(0.0f, 9.807f, 0.0f),
    acceleration(0.0f, 0.0f, 0.0f),
    force(0.0f, 0.0f, 0.0f),
    velocity(0.0f, 0.0f, 0.0f),
    mass(1.0f),
    floor_height(0.0f),
    bounce_factor(0.5f)
{
}

PhysicsComponent::~PhysicsComponent()
{
}

nlohmann::json PhysicsComponent::serialize()
{
    return {
        {"gravity", {gravity.x, gravity.y, gravity.z}},
        {"acceleration", {acceleration.x, acceleration.y, acceleration.z}},
        {"force", {force.x, force.y, force.z}},
        {"velocity", {velocity.x, velocity.y, velocity.z}},
        {"mass", mass},
        {"floor_height", floor_height},
        {"bounce_factor", bounce_factor},
    };
}
