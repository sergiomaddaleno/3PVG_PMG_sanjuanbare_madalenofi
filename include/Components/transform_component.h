#ifndef TRNS
#define TRNS

#include "..\include\input.h"
#include <../deps/glm/glm.hpp>


struct TransformComponent{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    float angle;

    TransformComponent() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 1.0f), scale(1.0f, 1.0f, 1.0f), angle(0.0f) {}

    TransformComponent(glm::vec3 position_, glm::vec3 rotation_, glm::vec3 scale_, float angle_)
        : position(position_), rotation(rotation_), scale(scale_), angle(angle_) {}

    TransformComponent(const TransformComponent& other)
        : position(other.position), rotation(other.rotation), scale(other.scale), angle(other.angle) {}

    TransformComponent& operator=(const TransformComponent& other) {
        if (this != &other) {
            position = other.position;
            rotation = other.rotation;
            scale = other.scale;
            angle = other.angle;
        }
        return *this;
    }

    void moveEntity(Input& input, float value) {

        if (input.isKeyDown(Input::LEFT)) {
            position.x -= value;
        }

        if (input.isKeyDown(Input::RIGHT)) {
            position.x += value;
        }

        if (input.isKeyDown(Input::UP)) {
            position.y += value;
        }

        if (input.isKeyDown(Input::DOWN)) {
            position.y -= value;
        }
    }

};

#endif // TRNS
