
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "mesh.h"
#include "render_system.h"
#include "input.h"
#include "ecs_manager.h"
#include "utils.h"
#include "shader.h"
#include "Components/light_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> toad_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

RenderSystem render_system;
RenderData rd;
ECSManager ecs;
Shader texture_shader, light_shader;

char* vs_texture = (char*)ReadFile("../assets/shaders/vs_texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/fs_texture.fs");

char* fs_light = (char*)ReadFile("../assets/shaders/fs_light.fs");
char* vs_light = (char*)ReadFile("../assets/shaders/vertex.vs");

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

    texture_shader.compile(vs_texture, fs_texture);
    light_shader.compile(vs_light, fs_light);

    auto e = ecs.create_entity();

    // ---

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();
    ecs.add_component_type<LightComponent>();

    // Light
    ecs.add_component<LightComponent>(e);
    ecs.add_component<TransformComponent>(e);
    ecs.get_component<LightComponent>(e)->shader = light_shader;
    ecs.get_component<TransformComponent>(e)->position = { 1.0f, -2.2f, 0.0f };
    //ecs.get_component<LightComponent>(e)->light_.dir_ = ;
    e++;
    // Mesh
    ecs.add_component<RenderComponent>(e);
    ecs.add_component<TransformComponent>(e);

    ecs.get_component<RenderComponent>(e)->shader = light_shader;
    ecs.get_component<RenderComponent>(e)->mesh_ = mario_mesh;
    ecs.get_component<TransformComponent>(e)->scale = { 0.4f, 0.4f, 0.4f };
    ecs.get_component<TransformComponent>(e)->position = {0.0f, -2.0f, 0.0f};

    //---
    
    render_system.initMesh(ecs);

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->colorWindow(color);

        rd.cam.update(input, dt, rd.w_width, rd.w_height);

        ecs.get_component<TransformComponent>(e)->moveEntity(input, dt);
        render_system.renderLight(ecs, rd);

        if (window->closePressed()) done = true;
    }

    return 0;
}





