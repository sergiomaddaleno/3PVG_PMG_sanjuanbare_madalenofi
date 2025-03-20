/**
 * @file physics_system.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include "../Systems/ecs_manager.h"

class PhysicsSystem {

public:
	PhysicsSystem(ECSManager& ecs);
	~PhysicsSystem();

	void init();
	void update();

private:
	ECSManager &ecs_;
};


#endif // !__PHYSICS_SYSTEM__
