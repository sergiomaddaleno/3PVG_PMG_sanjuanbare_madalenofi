#ifndef __PHYSICS_COMPONENT__
#define __PHYSICS_COMPONENT__

#include "../deps/nlohmann/json.hpp"

struct PhysicsComponent {

	float gravity = 9.807f;
	float acceleration = 0.0f;
	float force = 0.0f;
	float velocity = 1.0f;
	float mass = 1.0f;

	nlohmann::json serialize();
};

#endif // !__PHYSICS_COMPONENT__
