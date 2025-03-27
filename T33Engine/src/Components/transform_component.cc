#include "Components/transform_component.h"

void TransformComponent::moveEntity(Input& input, float dt, float speed)
{
    if (input.isKeyDown(Input::LEFT)) {
        position.x -= (speed * dt);
    }

    if (input.isKeyDown(Input::RIGHT)) {
        position.x += (speed * dt);
    }

    if (input.isKeyDown(Input::UP)) {
        position.y += (speed * dt);
    }

    if (input.isKeyDown(Input::DOWN)) {
        position.y -= (speed * dt);
    }
}

nlohmann::json TransformComponent::serialize() const
{
    return {
        {"Position", {position.x, position.y, position.z}},
        {"Rotation", {rotation.x, rotation.y, rotation.z, rotation.w}},
        {"Scale", {scale.x, scale.y, scale.z}}
    };
}

TransformComponent::TransformComponent() : 
    position(0.0f, 0.0f, 0.0f), 
    rotation(0.0f, 0.0f, 0.0f, 0.0f), 
    scale(1.0f, 1.0f, 1.0f),
    local_position(0.0f, 0.0f, 0.0f),
    local_rotation(0.0f, 0.0f, 0.0f, 0.0f),
    local_scale(1.0f, 1.0f, 1.0f)
{
}

TransformComponent::TransformComponent(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_, float angle_)
: position(position_), rotation(rotation_), scale(scale_),
  local_position(position_), local_rotation(rotation_), local_scale(scale_)
{}

TransformComponent& TransformComponent::operator=(const TransformComponent& other)
{
    if (this != &other) {
        position = other.position;
        rotation = other.rotation;
        scale = other.scale;
    }
    return *this;
}
