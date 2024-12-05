
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

char* vs_source_sky = (char*)ReadFile("../assets/shaders/vs_skybox.vs");
char* fs_source_sky = (char*)ReadFile("../assets/shaders/fs_skybox.fs");

char* dds_tex = ReadFile("../assets/textures/skybox/evening_field_octahedral.dds");

void ChangeShader(GLuint& program) {

    const char* shaders_name[]{ "Normal shader", "Texture Shader" };
    static int curr_item = 0;

    if (ImGui::Combo("Shaders", &curr_item, shaders_name, 2)) {
        switch (curr_item) {
        case 0:
            render_.setupShader(program, vs_normals, fs_normals);
            break;
        case 1:
            render_.setupShader(program, vs_texture, fs_texture);
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

void MyImGui(GLuint& program) {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Menu");

    ChangeShader(program);

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
    auto& program = ecs.get_component<RenderComponent>(j)->program_;

    render_.initMeshBuffers(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());
    render_.setupShader(program, vs_normals, fs_normals);

    // ---
    RenderComponent rc;
    rc.cubemap_tex = UploadTexture(dds_tex);

    render_.setupShader(rc.skybox_program_, vs_source_sky, fs_source_sky);
    // ---

    InitImGui(window->getWindow());

    double lastTime = glfwGetTime();
    bool done = false;
    while (!done) {
        window->updateWindow();
        double currentTime = glfwGetTime();
        float dt = float(currentTime - lastTime);
        
        cam.update(input,800,600,dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render_.renderSkyBox(cam, rc);
        render_.renderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>(), cam);

        MyImGui(program);

        if (window->closePressed()) done = true;
    }

    return 0;
}





