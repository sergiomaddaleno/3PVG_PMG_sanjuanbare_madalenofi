
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

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> toad_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

RenderSystem render_system;
RenderData rd;
ECSManager ecs;

char* vs_normals = (char*)ReadFile("../assets/shaders/normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/normals.fs");

char* vs_texture = (char*)ReadFile("../assets/shaders/texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/texture.fs");

int num_entities = 5000;
glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

Shader shader_tex, shader_normals;

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    // ---
    if (!toad_mesh->loadOBJ("../assets/obj/toad.obj", "../assets/textures/toad")) {
        return -1;
    }

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    if (!teapot_mesh->loadOBJ("../assets/obj/SM_Teapot.obj", "../assets/textures/toad")) {
       return -1;
    }

    auto j = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->mesh_ = mario_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 0.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

    j++;

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->mesh_ = toad_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 2.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 1.0f, 1.0f, 1.0f };

    for (int i = 2; i < num_entities; i++) {
        j++;
        ecs.add_component<RenderComponent>(j);
        ecs.add_component<TransformComponent>(j);
        ecs.get_component<RenderComponent>(j)->mesh_ = teapot_mesh;
        ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.5f, 0.5f, 0.5f };
    }

    //---
    shader_tex.compile(vs_texture, fs_texture);
    shader_normals.compile(vs_normals, fs_normals);

    ecs.get_component<RenderComponent>(0)->shader = shader_tex;
    ecs.get_component<RenderComponent>(1)->shader = shader_tex;

    for (int i = 2; i < num_entities; i++) {
        ecs.get_component<RenderComponent>(i)->shader = shader_normals;
    }
    //---

    render_system.initEntities(ecs);

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

        render_system.renderMesh(ecs, rd);

        if (window->closePressed()) done = true;
    }

    return 0;
}





