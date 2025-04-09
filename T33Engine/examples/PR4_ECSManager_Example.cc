
#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "Core/window.h"
#include "Core/mesh.h"
#include "Render/render_system.h"
#include "Core/input.h"
#include "Systems/ecs_manager.h"

#include "Components/render_component.h"
#include "Components/transform_component.h"
#include "utils.h"

auto teapot_mesh = std::make_unique<Mesh>();
auto toad_mesh = std::make_unique<Mesh>();
auto mario_mesh = std::make_unique<Mesh>();
ECSManager ecs;

int num_entities = 5000;
glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

Shader shader_tex;
RenderComponent rc;

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    RenderSystem render_system{ ecs };

    // ---
    if (!toad_mesh->loadOBJ("../stages/assets/obj/toad.obj", "../stages/assets/textures/toad")) {
        return -1;
    }

    if (!mario_mesh->loadOBJ("../stages/assets/obj/Mario.obj", "../stages/assets/textures/mario")) {
        return -1;
    }

    if (!teapot_mesh->loadOBJ("../stages/assets/obj/SM_Teapot.obj", "../stages/assets/textures/toad")) {
       return -1;
    }

    auto j = ecs.create_entity();
    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<TransformComponent>(j);
    ecs.add_component<RenderComponent>(j);
    ecs.get_component<RenderComponent>(j)->type_ = RT_Mesh;
    ecs.get_component<RenderComponent>(j)->mesh_ = std::move(mario_mesh);
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 0.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

    j++;

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->type_ = RT_Mesh;
    ecs.get_component<RenderComponent>(j)->mesh_ = std::move(toad_mesh);
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 2.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 1.0f, 1.0f, 1.0f };

    for (int i = 2; i < num_entities; i++) {
        j++;
        ecs.add_component<RenderComponent>(j);
        ecs.add_component<TransformComponent>(j);
        ecs.get_component<RenderComponent>(j)->type_ = RT_Mesh;
        ecs.get_component<RenderComponent>(j)->mesh_ = std::move(teapot_mesh);
        ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.5f, 0.5f, 0.5f };
    }

    initShader(shader_tex, "../stages/shaders/basic/texture.vs", "../stages/shaders/basic/texture.fs");

    render_system.initRenderTypes();
    render_system.shader = shader_tex;

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->colorWindow(color);

        float angle = (float)window->getTime();

        for (int i = 2; i < num_entities; i++) {
            ecs.get_component<TransformComponent>(i)->position = glm::vec3{ cos(angle + (float)i) * 8, (sin(angle + (float)i) * 8), -10.0f };
        }

        ecs.get_component<TransformComponent>(0)->moveEntity(input, dt);
        ecs.get_component<TransformComponent>(1)->moveEntity(input, dt);

        render_system.forwardRender(input, dt);

        if (window->closePressed(input)) done = true;
        lastTime = currentTime;

    }

    return 0;
}





