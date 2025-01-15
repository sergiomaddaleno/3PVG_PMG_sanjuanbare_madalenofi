
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "mesh.h"
#include "render.h"
#include "input.h"
#include "ecs_manager.h"
#include "utils.h"
#include "shader.h"
#include "Components/light_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> toad_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

Render render_;
RenderData rd;
ECSManager ecs;
Shader light_shader;

char* vs_light = (char*)ReadFile("../assets/shaders/vs_texture.vs");
char* fs_light = (char*)ReadFile("../assets/shaders/fs_texture.fs");

int num_entities = 5000;
glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    // ---

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    light_shader.compile(vs_light, fs_light);

    auto light_e = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();
    ecs.add_component_type<LightComponent>();

    ecs.add_component<LightComponent>(light_e);
    ecs.add_component<TransformComponent>(light_e);
    ecs.get_component<LightComponent>(light_e)->shader = light_shader;


    auto e = ecs.create_entity();

    ecs.add_component<RenderComponent>(e);
    ecs.add_component<TransformComponent>(e);
    ecs.get_component<RenderComponent>(e)->mesh_ = mario_mesh;

    //---
    
    render_.initMeshBuffers(ecs);

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->colorWindow(color);

        float angle = (float)window->getTime();

        ecs.get_component<TransformComponent>(j)->moveEntity(input, dt);

        render_.renderSystem(ecs, rd);

        if (window->closePressed()) done = true;
    }

    return 0;
}





