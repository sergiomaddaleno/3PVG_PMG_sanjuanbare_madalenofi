
#include "Render/deferred_shading.h"
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

float quadVertices[] = {
	// positions        // texture Coords
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Deferred::Deferred() : gBuffer(0), rboDepth(0),
	gPosition(0),gNormal(0), gAlbedoSpec(0), shadowMap(0),
	quadVAO(0), quadVBO(0), u_light_space(0),
	width_(800), height_(600)
{}

Deferred::~Deferred()
{
}

void Deferred::renderQuad()
{
	if (quadVAO == 0)
	{
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(1);
}

void Deferred::resize(unsigned int width, unsigned int height)
{
	if (width_ != width || height_ != height) {
		width_ = width;
		height_ = height;

		init();
	}
}

void Deferred::init()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width_, height_, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Deferred::render(ECSManager& ecs, Camera cam)
{

	// Geometry pass -------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClearColor(0.0, 0.0, 0.0, 1.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gbuffer.useProgram();

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

			glm::mat4 view = cam.getViewMatrix();
			glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

			gbuffer.setUniformValue(gbuffer.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
			deferred_shading.setUniformValue(deferred_shading.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));

			gbuffer.setUniformValue(gbuffer.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
			gbuffer.setUniformValue(gbuffer.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

			int tex_id = TextureID::TI_Albedo;
			gbuffer.setUniformValue(gbuffer.getUniformLocation("texture_diffuse"), ValueType::T_INT_1, &tex_id);			
		   
			GLuint screen_tex_loc = glGetUniformLocation(gbuffer.getProgram(), "u_screen_tex");
			glUniform1i(screen_tex_loc, 0);

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

	// Lightning pass -------------------------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	deferred_shading.useProgram();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

	int tex_id = TextureID::TI_Position;
	deferred_shading.setUniformValue(deferred_shading.getUniformLocation("gPosition"), ValueType::T_INT_1, &tex_id);
	tex_id = TextureID::TI_Normal;
	deferred_shading.setUniformValue(deferred_shading.getUniformLocation("gNormal"), ValueType::T_INT_1, &tex_id);
	tex_id = TextureID::TI_Albedo;
	deferred_shading.setUniformValue(deferred_shading.getUniformLocation("gAlbedoSpec"), ValueType::T_INT_1, &tex_id);
	
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ZERO);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	auto li = ecs.getComponentList<LightComponent>().begin();
	auto liend = ecs.getComponentList<LightComponent>().end();
	for (; li != liend; ++li) {
		if (li->has_value()) {
			auto& light_comp = *li;
			light_comp->light.setUniforms(deferred_shading);
			deferred_shading.setUniformValue(deferred_shading.getUniformLocation("u_camera_position"), ValueType::T_FLOAT_3, glm::value_ptr(cam.getPosition()));


			renderQuad();
		}
		glBlendFunc(GL_ONE, GL_ONE);
	}
	
	glDisable(GL_BLEND);
	glUseProgram(0);
	
	// ----------------------------------------------------------------------------------

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 1);

	glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 1);

}
