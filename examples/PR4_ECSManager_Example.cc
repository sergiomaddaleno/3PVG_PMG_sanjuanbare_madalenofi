#include "window.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

#include "ecs_manager.h"
#define M_PI 3.14159265358979323846

std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();
std::unique_ptr<Mesh> mesh_2 = std::make_unique<Mesh>();
std::unique_ptr<Mesh> mesh_3 = std::make_unique<Mesh>();

ECSManager ecs;
Render render_;
ShaderBuffers shaders = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shaders);

int main(int, char**) {

    if (!mesh_->LoadOBj("../assets/obj/SM_Teapot.obj")) {
        return -1;
    }

    if (!mesh_2->LoadOBj("../assets/obj/SM_Suzanne.obj")) {
        return -1;
    }

    if (!mesh_3->LoadOBj("../assets/obj/Mario.obj")) {
        return -1;
    }
    // ---
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();

    auto render = render_.make();
    // ---

    auto entidad_0 = ecs.create_entity();
    auto entidad_1 = ecs.create_entity();
    auto entidad_2 = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(entidad_0);
    ecs.add_component<TransformComponent>(entidad_0);
    ecs.get_component<RenderComponent>(entidad_0)->render_mesh = std::move(mesh_);
    ecs.get_component<TransformComponent>(entidad_0)->position = Vec3{ 3.0f, -1.0f, -5.0f };
    ecs.get_component<TransformComponent>(entidad_0)->rotation = Vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(entidad_0)->scale = Vec3{ 2.0f, 2.0f, 2.0f };

    ecs.add_component<RenderComponent>(entidad_1);
    ecs.add_component<TransformComponent>(entidad_1);
    ecs.get_component<RenderComponent>(entidad_1)->render_mesh = std::move(mesh_2);
    ecs.get_component<TransformComponent>(entidad_1)->position = Vec3{ -3.0f, 1.0f, -5.0f };
    ecs.get_component<TransformComponent>(entidad_1)->rotation = Vec3{ 1.0f, 0.0f, 0.0f };

    ecs.add_component<RenderComponent>(entidad_2);
    ecs.add_component<TransformComponent>(entidad_2);
    ecs.get_component<RenderComponent>(entidad_2)->render_mesh = std::move(mesh_3);
    ecs.get_component<TransformComponent>(entidad_2)->position = Vec3{ 2.0f, 2.0f, -5.0f };
    ecs.get_component<TransformComponent>(entidad_2)->rotation = Vec3{ 1.0f, 1.0f, 0.0f };

    bool done = false;
    while (!done) {
        window->UpdateWindow();
        
        float angle = (float)glfwGetTime();
        ecs.get_component<TransformComponent>(entidad_0)->angle = angle;
        ecs.get_component<TransformComponent>(entidad_1)->angle = angle;
        ecs.get_component<TransformComponent>(entidad_2)->angle = angle;

        render_.SetupShader(shaders);
        render_.RenderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());

        if (window->closePressed()) done = true;
    }

    return 0;
}





