#include "Render/outlining.h"
#include "Components/components.h"

#include "utils.h"

Outlining::Outlining(ECSManager& ecs) :
	ecs_(ecs)
{
}

Outlining::~Outlining()
{
}

void Outlining::initOutlineShader(std::string vertex_path, std::string fragment_path)
{
    initShader(shader, vertex_path, fragment_path);
}

void Outlining::init()
{
	
}

void Outlining::render(Shader& scene_shader, glm::mat4 camera_matrix)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

    auto ri = ecs_.getComponentList<RenderComponent>().begin();
    auto riend = ecs_.getComponentList<RenderComponent>().end();

    for (; ri != riend; ++ri) {
        if (ri->has_value()) {
            auto& render_comp = *ri;

            this->renderScene(*render_comp, scene_shader);
        }
    }

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    shader.useProgram();

    ri = ecs_.getComponentList<RenderComponent>().begin();
    riend = ecs_.getComponentList<RenderComponent>().end();

    auto ti = ecs_.getComponentList<TransformComponent>().begin();
    auto tiend = ecs_.getComponentList<TransformComponent>().end();

    int entity = 0;
    for (; ri != riend && ti != tiend; ++ri, ++ti, ++entity) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            glm::mat4 model = getWorldTransform(entity);
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform_comp->position);
            glm::mat4 rotation = glm::mat4_cast(transform_comp->rotation);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), transform_comp->scale);

            shader.setUniformValue(shader.getUniformLocation("u_outlining"), T_FLOAT_1, &outline_);
            shader.setUniformValue(shader.getUniformLocation("model"), T_FLOAT_4, &model);
            shader.setUniformValue(shader.getUniformLocation("translation"), T_FLOAT_4, &translation);
            shader.setUniformValue(shader.getUniformLocation("rotation"), T_FLOAT_4, &rotation);
            shader.setUniformValue(shader.getUniformLocation("scale"), T_FLOAT_4, &scale);
            shader.setUniformValue(shader.getUniformLocation("cam_matrix"), T_FLOAT_4, &camera_matrix);

            this->renderScene(*render_comp, this->shader);
        }
    }

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

glm::mat4 Outlining::getWorldTransform(int entity)
{
    if (!ecs_.has_component<TransformComponent>(entity))
        return glm::mat4(1.0f);

    auto& transform = ecs_.get_component<TransformComponent>(entity);
    glm::mat4 local = glm::translate(glm::mat4(1.0f), transform->position) *
        glm::mat4_cast(transform->rotation) *
        glm::scale(glm::mat4(1.0f), transform->scale);

    if (ecs_.has_component<HierarchyComponent>(entity)) {
        auto& hierarchy = ecs_.get_component<HierarchyComponent>(entity);
        if (hierarchy->hasParent()) {
            int parent = hierarchy->getParent();
            return getWorldTransform(parent) * local;
        }
    }

    return local;
}

void Outlining::renderScene(RenderComponent& rc, Shader& shader)
{
    switch (rc.type_) {
    case RenderType::RT_Mesh:
    {
        for (auto& mesh : rc.mesh_->meshes) {
            rc.mesh_->draw();
        }
    }
    break;
    case RenderType::RT_Terrain:
    {
        rc.terrain_->draw();
    }
    break;
    case RenderType::RT_Sphere:
    {
        rc.sphere_->draw();
    }
    break;
    case RenderType::RT_Surface:
    {
        rc.surface_->draw();
    }
    break;
    case RenderType::RT_Cube8v:
    case RenderType::RT_Cube24v:
    {
        rc.cube_->draw();
    }
    break;
    case RenderType::RT_Quad:
    {
        rc.quad_->draw();
    }
    break;
    case RenderType::RT_Water:
        rc.water_->draw();
        break;
    }

    shader.useProgram();
}

