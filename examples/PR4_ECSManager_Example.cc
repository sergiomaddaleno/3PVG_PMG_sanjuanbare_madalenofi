#include "window.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"
#include "..\include\input.h"

#include "ecs_manager.h"
#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> teapot_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mesh_2 = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

ECSManager ecs;
Render render_;
ShaderBuffers shaders_tex = render_.initShader("../assets/shaders/vs_texture.vs", "../assets/shaders/fs_texture.fs", shaders_tex);
ShaderBuffers shaders_norm = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders_norm);

vec2 arrow_dir = { 0.0f, 0.0f };
float move_speed = 0.05;

vec4 color = { 0.5f, 0.5f, 0.5f, 1.0f };


int main(int, char**) {
   

    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();

    auto render = render_.make();

    // ---

    Input input{ window->GetWindow() };

    // ---
    if (!mario_mesh->LoadOBj("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }
    
   if (!teapot_mesh->LoadOBj("../assets/obj/SM_Teapot.obj")) {
       return -1;
   }

    auto j = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->render_mesh = mario_mesh;
    ecs.get_component<TransformComponent>(j)->position = Vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->rotation = Vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = Vec3{ 0.7f, 0.7f, 0.7f };
    
    for (int i = 1; i < 50; i++) {
        j++;
        ecs.add_component<RenderComponent>(j);
        ecs.add_component<TransformComponent>(j);
        ecs.get_component<RenderComponent>(j)->render_mesh = teapot_mesh;
        ecs.get_component<TransformComponent>(j)->rotation = Vec3{ 0.0f, 1.0f, 0.0f };
        ecs.get_component<TransformComponent>(j)->scale = Vec3{ 0.5f, 0.5f, 0.5f };        
    }
    
    bool done = false;
    while (!done) {
        window->UpdateWindow();
        window->ColorWindow(color);
        
        float angle = (float)glfwGetTime();
        
        for (int i = 1; i < 50; i++) {
            ecs.get_component<TransformComponent>(i)->angle = angle;
            ecs.get_component<TransformComponent>(i)->position = Vec3{ cos(angle + (float)i) * 10, (sin(angle + (float)i) * 10) + 3.0f, -10.0f };
        }
 
        ecs.get_component<TransformComponent>(0)->MoveEntity(input, 0.1f);

        render_.SetupShader(shaders_tex);
        render_.RenderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());

        if (window->closePressed()) done = true;
    }

    return 0;
}





