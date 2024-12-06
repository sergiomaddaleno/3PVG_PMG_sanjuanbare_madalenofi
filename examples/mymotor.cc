
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

std::shared_ptr<Mesh> car_mesh = std::make_shared<Mesh>();
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

GLuint normalShaderProgram = 0;
GLuint textureShaderProgram = 0;

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
    window->setResizeCallback();

    render_.make();

    Input input{ window->getWindow() };

    // ---
    if (!car_mesh->loadOBJ("../assets/obj/LMcQueen.obj", "../assets/textures/cars")) {
        return -1;
    }

    auto j = ecs.create_entity();

    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(j);
    ecs.add_component<TransformComponent>(j);
    ecs.get_component<RenderComponent>(j)->mesh_ = car_mesh;
    ecs.get_component<TransformComponent>(j)->position = glm::vec3{ 0.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(j)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };
    
    render_.setupShader(ecs.get_component<RenderComponent>(j)->program_, vs_normals, fs_normals);

    // ---
    RenderComponent rc;
    rc.cubemap_tex = UploadTexture(dds_tex);
    render_.setupShader(rc.skybox_program_, vs_source_sky, fs_source_sky);
    // ---

    render_.initMeshBuffers(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>());

    InitImGui(window->getWindow());

    double lastTime = glfwGetTime();
    bool done = false;
    while (!done) {
        double currentTime = glfwGetTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        
        cam.update(input, window->getWidth(), window->getHeight(), dt);

        render_.renderSkyBox(rc, cam, window->getWidth(), window->getHeight());
        render_.renderSystem(ecs.getComponentList<RenderComponent>(), ecs.getComponentList<TransformComponent>(), cam, window->getWidth(), window->getHeight());

        MyImGui(ecs.get_component<RenderComponent>(j)->program_);

        if (window->closePressed()) done = true;
    }

    return 0;
}





