
#include "Render/shadow_map.h"

#include "Systems/ecs_manager.h"
#include "Core/shader.h"

#include "Geometry/sphere.h"
#include "Geometry/terrain.h"
#include "Geometry/cube.h"
#include "Geometry/quad.h"

#include "Components\transform_component.h"
#include "Components\render_component.h"
#include "Components\light_component.h"
#include "global_data.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <memory> 
#include <iostream>
#include "Render/deferred.h"



Deferred::Deferred() : gBuffer(0), gPosition(0),
gNormal(0), gColorSpec(0), deferred_width_(800), deferred_height_(600)
{

}

Deferred::~Deferred()
{
}

void Deferred::initDefered()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, deferred_width_, deferred_height_, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, deferred_width_, deferred_height_, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, deferred_width_, deferred_height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
}

void Deferred::render(ECSManager& ecs, Shader shader, LightComponent lc)
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClearColor(0.0, 0.0, 0.0, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.useProgram();
	auto ri = ecs.getComponentList<RenderComponent>().begin();
	auto ti = ecs.getComponentList<TransformComponent>().begin();
	auto riend = ecs.getComponentList<RenderComponent>().end();
	auto tiend = ecs.getComponentList<TransformComponent>().end();

	for (; ri != riend && ti != tiend; ++ri, ++ti) {
		if (ri->has_value() && ti->has_value()) {
			auto& render_comp = *ri;
			auto& transform_comp = *ti;

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, transform_comp->position);
			model = model * glm::mat4_cast(transform_comp->rotation);
			model = glm::scale(model, transform_comp->scale);
			shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));

			switch (render_comp->type_) {
			case RenderType::RT_Mesh:
				for (auto& mesh : render_comp->mesh_->meshes) {
					render_comp->mesh_->draw();
				}
				break;
			case RenderType::RT_Terrain:
				render_comp->terrain_->draw();
				break;
			case RenderType::RT_Sphere:
				render_comp->sphere_->draw();
				break;
			case RenderType::RT_Cube8v:
			case RenderType::RT_Cube24v:
				render_comp->cube_->draw();
				break;
			case RenderType::RT_Quad:
				render_comp->quad_->draw();
				break;
			}
		}
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);

	

}
