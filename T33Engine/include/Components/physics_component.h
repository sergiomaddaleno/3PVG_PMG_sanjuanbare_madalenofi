#ifndef __PHYSICS_COMPONENT__
#define __PHYSICS_COMPONENT__

#include <../deps/glm/glm.hpp>
#include "../deps/nlohmann/json.hpp"

struct PhysicsComponent {

    PhysicsComponent();
    ~PhysicsComponent();

    glm::vec3 gravity;
    glm::vec3 acceleration;
    glm::vec3 force; 
    glm::vec3 velocity; 
    float mass;

    float floor_height;
    float bounce_factor;

	nlohmann::json serialize();
};

#endif // !__PHYSICS_COMPONENT__
