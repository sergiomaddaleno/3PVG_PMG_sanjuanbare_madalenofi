#ifndef __TRANSFORM_COMPONENT__
#define __TRANSFORM_COMPONENT__

#include "Core/input.h"
#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "../deps/nlohmann/json.hpp"

struct TransformComponent {

    glm::vec3 position;
    glm::quat rotation;  
    glm::vec3 scale;

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

    // ---

    nlohmann::json serialize() const;

    TransformComponent();
    TransformComponent(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_, float angle_);
    TransformComponent& operator=(const TransformComponent& other);
};

#endif // __TRANSFORM_COMPONENT__
