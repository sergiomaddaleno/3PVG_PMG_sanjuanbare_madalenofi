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
#include "Render/render_system.h"
#include "Core/shader.h"

class PhysicsSystem {
public:
    PhysicsSystem(ECSManager& ecs, RenderSystem& rs);
    ~PhysicsSystem();

    void update(float dt);

    double dt_;
    bool stop_physics_;

private:
    ECSManager& ecs_;
    RenderSystem& rs_; 
};


#endif // !__PHYSICS_SYSTEM__
