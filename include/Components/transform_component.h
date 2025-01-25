#ifndef TRNS
#define TRNS

#include "..\include\input.h"
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
            {"position", {position.x, position.y, position.z}},
            {"rotation", {rotation.x, rotation.y, rotation.z, rotation.w}},
            {"scale", {scale.x, scale.y, scale.z}}
        };
    }

    nlohmann::json deSerialize(nlohmann::json j) {

        std::vector<float> temp_float;

        temp_float = j["position"].get<std::vector<float>>();
        position = { temp_float[0], temp_float[1], temp_float[2] };

        temp_float = j["rotation"].get<std::vector<float>>();
        rotation = { temp_float[0], temp_float[1], temp_float[2], temp_float[3] };

        temp_float = j["scale"].get<std::vector<float>>();
        scale = { temp_float[0], temp_float[1], temp_float[2] };

        return j;
    }

    // ----------------------------------

    TransformComponent() : position(0.0f, 0.0f, 0.0f), rotation(1.0f, 0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f){}

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
