
#include "render/physics_system.h"
#include "Components/components.h"

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

	for (; pi != piend && ti != tiend; ++pi, ++ti) {
		if (pi->has_value() && ti->has_value()) {
			auto& physics_comp = *pi;
			auto& transf_comp = *ti;


		}
	}
}
