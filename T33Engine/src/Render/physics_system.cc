
#include "render/physics_system.h"
#include "Components/components.h"

PhysicsSystem::PhysicsSystem(ECSManager& ecs, RenderSystem& rs) : 
	ecs_(ecs),
	rs_(rs), dt_(0.0),
	stop_physics_(false)
{
}

PhysicsSystem::~PhysicsSystem()
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

			if (!stop_physics_) {

				physics_comp->acceleration = physics_comp->force / physics_comp->mass;
				physics_comp->acceleration -= physics_comp->gravity;
				physics_comp->velocity += physics_comp->acceleration * (float)dt_;
				transf_comp->position += physics_comp->velocity * (float)dt_;

				if (transf_comp->position.y <= physics_comp->floor_height) {
					transf_comp->position.y = physics_comp->floor_height;
					physics_comp->velocity.y *= -physics_comp->bounce_factor;
				}
			}
		}
	}
}
