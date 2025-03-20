
#include "render/physics_system.h"
#include "Components/components.h"
#include "global_data.h"

PhysicsSystem::PhysicsSystem(ECSManager& ecs) : 
	ecs_(ecs)
{
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::init()
{
}

void PhysicsSystem::update()
{
	auto pi = ecs_.getComponentList<PhysicsComponent>().begin();
	auto piend = ecs_.getComponentList<PhysicsComponent>().end();

	auto ti = ecs_.getComponentList<TransformComponent>().begin();
	auto tiend = ecs_.getComponentList<TransformComponent>().end();

	auto ri = ecs_.getComponentList<RenderComponent>().begin();
	auto riend = ecs_.getComponentList<RenderComponent>().end();

	for (; pi != piend && ti != tiend && ri != riend; ++pi, ++ti, ++ri) {
		if (pi->has_value() && 
			ti->has_value() && 
			ri->has_value()){
			auto& physics_comp = *pi;
			auto& transf_comp = *ti;
			auto& render_comp = *ri;

			physics_comp->acceleration = physics_comp->force / physics_comp->mass;
			physics_comp->velocity -= (physics_comp->acceleration - physics_comp->gravity) * global.dt;

			transf_comp->position.y -= physics_comp->velocity * global.dt;
		}
	}
}
