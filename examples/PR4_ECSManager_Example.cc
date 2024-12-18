
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

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> toad_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

Render render_;
RenderData rd;
ECSManager ecs;

char* vs_normals = (char*)ReadFile("../assets/shaders/vs_normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/fs_normals.fs");

char* vs_texture = (char*)ReadFile("../assets/shaders/vs_texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/fs_texture.fs");

int num_entities = 5000;
glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

GLuint program_tex = 0;
GLuint program_normals = 0;

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();

    auto render = render_.make();

    Input input{ window->getWindow() };

    // ---
    if (!toad_mesh->loadOBJ("../assets/obj/toad.obj", "../assets/textures/toad")) {
        return -1;
    }

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    if (!teapot_mesh->loadOBJ("../assets/obj/SM_Teapot.obj")) {
       return -1;
    }

    auto j = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->mesh_ = mario_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 0.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };

    j++;

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->mesh_ = toad_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 2.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 1.0f, 1.0f, 1.0f };

    for (int i = 2; i < num_entities; i++) {
        j++;
        ecs.add_component<RenderComponent>(j);
        ecs.add_component<TransformComponent>(j);
        ecs.get_component<RenderComponent>(j)->mesh_ = teapot_mesh;
        ecs.get_component<TransformComponent>(j)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
        ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.5f, 0.5, 0.5f };
    }

    //---
    render_.setupShader(program_tex, vs_texture, fs_texture);
    render_.setupShader(program_normals, vs_normals, fs_normals);

    ecs.get_component<RenderComponent>(0)->program_ = program_tex;
    ecs.get_component<RenderComponent>(1)->program_ = program_tex;

    for (int i = 2; i < num_entities; i++) {
        ecs.get_component<RenderComponent>(i)->program_ = program_normals;
    }
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

        for (int i = 2; i < num_entities; i++) {
            ecs.get_component<TransformComponent>(i)->angle = angle;
            ecs.get_component<TransformComponent>(i)->position = glm::vec3{ cos(angle + (float)i) * 8, (sin(angle + (float)i) * 8), -10.0f };
        }

        ecs.get_component<TransformComponent>(0)->moveEntity(input, dt);
        ecs.get_component<TransformComponent>(1)->moveEntity(input, dt);

        render_.renderSystem(ecs, rd);

        if (window->closePressed()) done = true;
    }

    return 0;
}





