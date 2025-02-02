#ifndef TRNS
#define TRNS

#include "Core/input.h"
#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include <cmath>

#include "../deps/nlohmann/json.hpp"

#ifndef M_PI
const float M_PI = 3.14159265358979323846f;
#endif

struct TransformComponent {

    glm::vec3 position;
    glm::quat rotation;  
    glm::vec3 scale;

    // move entity with input
    void moveEntity(Input& input, float dt, float speed = 0.001f) {

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

    // Save data
    nlohmann::json serialize() const {
        return {
            {"Position", {position.x, position.y, position.z}},
            {"Rotation", {rotation.x, rotation.y, rotation.z, rotation.w}},
            {"Scale", {scale.x, scale.y, scale.z}}
        };
    }

    // ----------------------------------

    TransformComponent() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f){}

    TransformComponent(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_, float angle_)
        : position(position_), rotation(rotation_), scale(scale_){}

    TransformComponent& operator=(const TransformComponent& other) {
        if (this != &other) {
            position = other.position;
            rotation = other.rotation;
            scale = other.scale;
        }
        return *this;
    }
};

#endif
