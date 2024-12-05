
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\input.h"
#include "camera.h"
#include "ecs_manager.h"
#include "utils.h"

#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> toad_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

ECSManager ecs;
Render render_;
Camera cam;

char* vs_normals = (char*)ReadFile("../assets/shaders/vs_normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/fs_normals.fs");

char* vs_texture = (char*)ReadFile("../assets/shaders/vs_texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/fs_texture.fs");

int num_entities = 50;
glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

int main(int, char**) {
   
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();

    auto render = render_.make();

    // ---

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

    render_.initMeshBuffers(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());
    render_.setupShader(ecs.get_component<RenderComponent>(0)->program_, vs_texture, fs_texture);
    render_.setupShader(ecs.get_component<RenderComponent>(1)->program_, vs_texture, fs_texture);

    for (int i = 2; i < num_entities; i++) {
        render_.setupShader(ecs.get_component<RenderComponent>(i)->program_, vs_normals, fs_normals);
    }

    double lastTime = glfwGetTime();
    bool done = false;
    while (!done) {

        window->colorWindow(color);
        double currentTime = glfwGetTime();
        float dt = float(currentTime - lastTime);

        float angle = (float)glfwGetTime();

        cam.update(input, 800, 600, dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 2; i < 50; i++) {
            ecs.get_component<TransformComponent>(i)->angle = angle;
            ecs.get_component<TransformComponent>(i)->position = glm::vec3{ cos(angle + (float)i) * 8, (sin(angle + (float)i) * 8), -10.0f };
        }

        ecs.get_component<TransformComponent>(0)->moveEntity(input, 0.1f);
        ecs.get_component<TransformComponent>(1)->moveEntity(input, 0.1f);

        render_.renderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>(), cam);


        window->updateWindow();

        if (window->closePressed()) done = true;
    }

    return 0;
}





