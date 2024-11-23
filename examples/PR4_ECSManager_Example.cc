#include "window.h"
#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include "..\include\mesh.h"
#include "..\include\render.h"
#include "..\include\EcsComponents\transform_component.h"
#include "..\include\EcsComponents\render_component.h"

#include "ecs_manager.h"
#define M_PI 3.14159265358979323846

static const char* vertex_shader_text =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"out vec3 normal;\n"
"out vec3 FragPos;\n"
"out vec3 pos;\n"
"out vec2 uv;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"    normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    pos = aPos;\n"
"    uv = aTexCoord;\n"
"    FragPos = aPos;\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 normal;\n"
"in vec3 FragPos;\n"
"in vec3 pos;\n"
"in vec2 uv;\n"
"void main() {\n"
"    vec3 norm = normalize(normal);\n"
"    FragColor = vec4(norm, 1.0);\n"
"}\n";

std::unique_ptr<Mesh> mesh_ = std::make_unique<Mesh>();
std::unique_ptr<Mesh> mesh_2 = std::make_unique<Mesh>();
std::unique_ptr<Mesh> mesh_3 = std::make_unique<Mesh>();

Render render_;
ECSManager ecs;

int main(int, char**) {

    if (!mesh_->LoadOBj("../assets/SM_Teapot.obj")) {
        return -1;
    }

    if (!mesh_2->LoadOBj("../assets/SM_Suzanne.obj")) {
        return -1;
    }

    if (!mesh_3->LoadOBj("../assets/Mario.obj")) {
        return -1;
    }
    // ---
    glfwInit();

    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();
    // ---

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


        render_.SetupShader(vertex_shader_text, fragment_shader_text);
        render_.RenderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());
        

        if (window->closePressed()) done = true;
    }

    return 0;
}





