
#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "Core/window.h"
#include "Core/mesh.h"
#include "Render/render_system.h"
#include "Core/input.h"
#include "Systems/ecs_manager.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include "Components/render_component.h"
#include "Components/transform_component.h"
#include "utils.h"

auto teapot_mesh = std::make_shared<Mesh>();
auto toad_mesh = std::make_shared<Mesh>();
auto mario_mesh = std::make_shared<Mesh>();

ECSManager ecs;
Camera cam;
Shader shader_tex;
RenderComponent rc;

glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };
int num_entities = 5000;

void renderScene(RenderComponent& rc, Shader& shader)
{
    switch (rc.type_) {
    case RenderType::RT_Mesh:
    {
        for (auto& mesh : rc.mesh_->meshes) {
            rc.mesh_->wave_.setUniforms(shader, (float)glfwGetTime());
            rc.mesh_->draw();
        }
    }
    break;
    }
}

void setUniforms(TransformComponent& tc, Shader& shader, Camera& cam)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    shader.setUniformValue(shader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    shader.setUniformValue(shader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    shader.setUniformValue(shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    int tex_id = TextureID::TI_Diffuse;
    shader.setUniformValue(shader.getUniformLocation("u_texture"), ValueType::T_INT_1, &tex_id);
}

void forwardRender(Shader shader,Camera cam) {

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
   
    shader.useProgram();

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    for (; ri != riend && ti != tiend; ++ri, ++ti) {
        if (ri->has_value() && ti->has_value()) {
            auto& render_comp = *ri;
            auto& transform_comp = *ti;

            setUniforms(*transform_comp, shader,cam);
            renderScene(*render_comp, shader);
        }
    }

    glUseProgram(0);
}

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW(); 

    Input input{ window->getWindow() };

    // ---
    if (!toad_mesh->loadOBJ("../stages/assets/obj/toad.obj", "../stages/assets/textures/toad")) {
        return -1;
    }

    if (!mario_mesh->loadOBJ("../stages/assets/obj/Mario.obj", "../stages/assets/textures/mario")) {
        return -1;
    }

    if (!teapot_mesh->loadOBJ("../stages/assets/obj/SM_Teapot.obj")) {
       return -1;
    }

    initShader(shader_tex, "../stages/shaders/basic/texture.vs", "../stages/shaders/basic/texture.fs");

    auto j = ecs.create_entity() + 1;

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<TransformComponent>(j);
    ecs.add_component<RenderComponent>(j);
    ecs.get_component<RenderComponent>(j)->type_ = RT_Mesh;
    ecs.get_component<RenderComponent>(j)->mesh_ = mario_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ -1.0f, 4.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

    j++;

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->type_ = RT_Mesh;
    ecs.get_component<RenderComponent>(j)->mesh_ = toad_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 1.0f, 4.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 1.0f, 1.0f, 1.0f };

    for (int i = 2; i < num_entities ; i++) {
        j++;
        ecs.add_component<RenderComponent>(j);
        ecs.add_component<TransformComponent>(j);
        ecs.get_component<RenderComponent>(j)->type_ = RT_Mesh;
        ecs.get_component<RenderComponent>(j)->mesh_ = teapot_mesh;
        ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };
    }
   
    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();

    for (; ri != riend; ++ri) {
        if (ri->has_value()) {
            auto& render_comp = *ri;

            switch (render_comp->type_) {
            case RenderType::RT_Mesh:
                render_comp->mesh_->bind();
                break;
            }
        }
    }

    cam.setViewDir(glm::vec3(0, 0, -1.0));
    cam.lockViewDirection(true);
    cam.lockMovement(true);

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->colorWindow(color);

        cam.update(input,dt);

        float angle = (float)window->getTime();
        for (int i = 2; i < num_entities; i++) {
            ecs.get_component<TransformComponent>(i)->position = glm::vec3{ cos(angle + (float)i) * 8, 5.0f + (sin(angle + (float)i) * 8), -10.0f };
        }

        ecs.get_component<TransformComponent>(0)->moveEntity(input, dt);
        ecs.get_component<TransformComponent>(1)->moveEntity(input, dt);

        forwardRender(shader_tex,cam);

        if (window->closePressed(input)) done = true;
        lastTime = currentTime;

    }

    return 0;
}





