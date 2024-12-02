
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\input.h"
#include "ecs_manager.h"

#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> toad_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

ECSManager ecs;
Render render_;
ShaderBuffers shaders_tex = render_.initShader("../assets/shaders/vs_texture.vs", "../assets/shaders/fs_texture.fs", shaders_tex);
ShaderBuffers shaders_norm = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders_norm);

glm::vec2 arrow_dir = { 0.0f, 0.0f };
float move_speed = 0.05;

glm::vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };

int main(int, char**) {
   

    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();

    auto render = render_.make();

    // ---

    Input input{ window->getWindow() };

    // ---
    if (!toad_mesh->loadOBJ("../assets/obj/Toad.obj", "../assets/textures/toad")) {
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

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->mesh_ = toad_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ -1.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 1.2f, 1.2f, 1.2f };
    
    for (int i = 2; i < 50; i++) {
        j++;
        ecs.add_component<RenderComponent>(j);
        ecs.add_component<TransformComponent>(j);
        ecs.get_component<RenderComponent>(j)->mesh_ = teapot_mesh;
        ecs.get_component<TransformComponent>(j)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
        ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 1.3f, 1.3f, 1.3f };
    }
    
    bool done = false;

    render_.initMeshBuffers(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());

    while (!done) {
        window->updateWindow();
        window->colorWindow(color);
        
        float angle = (float)glfwGetTime();
        
        for (int i = 2; i < 50; i++) {
            ecs.get_component<TransformComponent>(i)->angle = angle;
            ecs.get_component<TransformComponent>(i)->position = glm::vec3{ cos(angle + (float)i) * 8, (sin(angle + (float)i) * 8), -10.0f };
        }
        
        ecs.get_component<TransformComponent>(0)->moveEntity(input, 0.1f);
        ecs.get_component<TransformComponent>(1)->moveEntity(input, 0.1f);

        render_.setupShader(shaders_tex);
        render_.renderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());

        if (window->closePressed()) done = true;
    }

    return 0;
}





