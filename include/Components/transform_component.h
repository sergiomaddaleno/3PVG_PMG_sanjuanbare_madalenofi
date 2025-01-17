#ifndef TRNS
#define TRNS

#include "..\include\input.h"
#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include <cmath>
#ifndef M_PI
const float M_PI = 3.14159265358979323846f;
#endif

struct TransformComponent {

    glm::vec3 position;
    glm::quat rotation;  
    glm::vec3 scale;
    float angle; 


    TransformComponent() : position(0.0f, 0.0f, 0.0f), rotation(glm::quat(0.02f, 0.01f, 0.001f, 0.99f)), scale(1.0f, 1.0f, 1.0f), angle(0.0f) {}

    TransformComponent(glm::vec3 position_, glm::vec3 scale_, float angle_)
        : position(position_), scale(scale_), angle(angle_) {
        rotation = glm::quat(glm::vec3(glm::radians(angle_), 0.0f, 0.0f)); 
    }

    // move entity witj input
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

    glm::mat4 getRotationMatrix() const {
        return glm::mat4_cast(rotation);  
    }

    glm::mat4 getTransformMatrix() const {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);  
        model *= getRotationMatrix(); 
        model = glm::scale(model, scale); 
        return model;
    }

    TransformComponent& operator=(const TransformComponent& other) {
        if (this != &other) {
            position = other.position;
            rotation = other.rotation;
            scale = other.scale;
            angle = other.angle;
        }
        return *this;
    }
};

#endif
