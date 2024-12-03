
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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
ShaderBuffers shader = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shader);

glm::vec2 arrow_dir = { 0.0f, 0.0f };
float move_speed = 0.05;

/*void ChangePostprocess()
{
    const char* shaders_name[]{ "Default", "Negative", "Blur", "Grey","Waves" };
    static int curr_item = 0;
    if (ImGui::Combo("Postprocess", &curr_item, shaders_name, 5)) {
        switch (curr_item) {
        case 0:
            GameState.mat_postprocess->init("./shaders/vertex.vs", "./shaders/fragment.fs");
            break;
        case 1:
            GameState.mat_postprocess->init("./shaders/postprocess/vertex_pp.vs", "./shaders/postprocess/negative.fs");
            break;
        case 2:
            GameState.mat_postprocess->init("./shaders/postprocess/vertex_pp.vs", "./shaders/postprocess/blur.fs");
            break;
        case 3:
            GameState.mat_postprocess->init("./shaders/postprocess/vertex_pp.vs", "./shaders/postprocess/Grey.fs");
            break;
        case 4:
            GameState.mat_postprocess->init("./shaders/postprocess/vertex_pp.vs", "./shaders/postprocess/waves.fs");
            break;
        }
    }
}*/

void ChangeShader() {

    const char* shaders_name[]{ "Normal shader", "Texture Shader" };
    static int curr_item = 0;

    if (ImGui::Combo("Shaders", &curr_item, shaders_name, 2)) {
        switch (curr_item) {
        case 0:
            shader = render_.initShader("../assets/shaders/vertex_normals.vs", "../assets/shaders/fragment_normals.fs", shader);
            break;
        case 1:
            shader = render_.initShader("../assets/shaders/vs_texture.vs", "../assets/shaders/fs_texture.fs", shader);
            break;
        }
    }
}

void InitImGui(GLFWwindow* w) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    int width, height;
    glfwGetFramebufferSize(w, &width, &height);
    io.DisplaySize = ImVec2((float)width, (float)height);

    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void MyImGui() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Menu");

    ChangeShader();

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main(int, char**) {

    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();

    auto render = render_.make();

    // ---

    Input input{ window->getWindow() };

    // ---
    if (!toad_mesh->loadOBJ("../assets/obj/LMcQueen.obj", "../assets/textures/cars")) {
        return -1;
    }

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
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

    render_.initMeshBuffers(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());


    InitImGui(window->getWindow());
    bool done = false;
    while (!done) {
        window->updateWindow();

        ecs.get_component<TransformComponent>(0)->moveEntity(input, 0.1f);

        render_.setupShader(shader);
        render_.renderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());

        MyImGui();

        if (window->closePressed()) done = true;
    }

    return 0;
}





