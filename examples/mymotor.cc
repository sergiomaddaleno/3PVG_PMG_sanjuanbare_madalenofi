
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include "window.h"
#include "mesh.h"
#include "render.h"
#include "shader.h"
#include "input.h"
#include "post_processing.h"
#include "utils.h"
#include "skybox.h"

#include "ecs_manager.h"
#include "scripting.h"
#include "Components/scripting_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> car_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

Render render_;
RenderData rd;
LuaScript ls;
Skybox skybox;
Texture skyboxTex;
ECSManager ecs;
PostProcessing postProcc;

auto e = ecs.create_entity();

char* vs_normals = (char*)ReadFile("../assets/shaders/vs_normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/fs_normals.fs");
char* vs_texture = (char*)ReadFile("../assets/shaders/vs_texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/fs_texture.fs");

char* vs_source_sky = (char*)ReadFile("../assets/shaders/vs_skybox.vs");
char* fs_source_sky = (char*)ReadFile("../assets/shaders/fs_skybox.fs");

char* vertex_pp = (char*)ReadFile("../assets/shaders/postprocess/vertex_pp.vs");
char* grey_fs = (char*)ReadFile("../assets/shaders/postprocess/grey.fs");
char* negative_fs = (char*)ReadFile("../assets/shaders/postprocess/negative.fs");
char* waves_fs = (char*)ReadFile("../assets/shaders/postprocess/waves.fs");
char* blur_fs = (char*)ReadFile("../assets/shaders/postprocess/blur.fs");
char* kernel_fs = (char*)ReadFile("../assets/shaders/postprocess/kernel.fs");

char* dds_file = ReadFile("../assets/textures/skybox/sky.dds");
char* lua_file = ReadFile("../scripts/settings.lua");

Shader normals_shader, texture_shader;
Shader grey_sh, negative_sh, waves_sh, blur_sh, kernel_sh;

void ChangeShader() {

    const char* shaders_name[]{ "Texture Shader", "Normals shader"};
    static int curr_item = 0;

    if (ImGui::Combo("Shaders", &curr_item, shaders_name, 2)) {
        switch (curr_item) {
        case 0:
            ecs.get_component<RenderComponent>(e)->shader = texture_shader;
            break;
        case 1:
            ecs.get_component<RenderComponent>(e)->shader = normals_shader;
            break;
        }
    }
}

void ChangePostProcess() {

    const char* pp_name[]{ "None", "Grey", "Negative", "Waves", "Blur", "Kernel types"};
    static int curr_item = 0;

    if (ImGui::Combo("Post Process", &curr_item, pp_name, 6)) {
    }

    switch (curr_item) {
    case 0: break;

    case 1: 
        postProcc.render(grey_sh, Effect::Grey);
        ImGui::DragFloat("Grey", &postProcc.grey_, 0.01f, 0.0f, 1.0f, "%.3f");
        break;
    case 2: 
        postProcc.render(negative_sh, Effect::Negative);
        ImGui::DragFloat("Negative", &postProcc.negative_, 0.01f, 0.5f, 1.0f, "%.3f"); 
        break;
    case 3:
        postProcc.render(waves_sh, Effect::Waves);
        ImGui::DragFloat("Waves", &postProcc.waves_, 0.001f, 0.00f, 0.09f, "%.3f");
        break;
    case 4:
        postProcc.render(blur_sh, Effect::Blur);
        ImGui::DragFloat("Blur", &postProcc.blur_, 0.00001f, 0.00f, 0.035f, "%.3f");
        break;
    case 5:
        postProcc.render(kernel_sh, Effect::Kernel);
        ImGui::DragInt("Kernel types", &postProcc.kernel_types_, 0.01f, 0, 2, "%d");
        break;
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
    ChangePostProcess();
    
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main(int, char**) {

    //lua script settings
    ls.run(lua_file);

    ls.get_table("graphics");
    float fov = ls.get_float("fov");
    ls.pop();

    ls.get_table("window");
    int w_width = ls.get_integer("window_width");
    int w_height = ls.get_integer("window_height");
    bool full_screen = ls.get_bool("full_screen");
    ls.pop();

    rd.w_width = w_width;
    rd.w_height = w_height;
    rd.fov = fov;
    // --

    auto window = Window::make(rd.w_width, rd.w_height, "Window ", full_screen);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();

    Input input{ window->getWindow()};

    // ---
    if (!car_mesh->loadOBJ("../assets/obj/LMcQueen.obj", "../assets/textures/cars")) {
        return -1;
    }

    texture_shader.compile(vs_texture, fs_texture);
    normals_shader.compile(vs_normals, fs_normals);

    //Skybox
    if (skyboxTex.loadSkyboxTex(dds_file)) {
        rd.render_comp.skybox_tex = skyboxTex.getTexture();
    }

    rd.render_comp.skybox_shader.compile(vs_source_sky, fs_source_sky);

    skybox.init();
    // ---

    //ECS
    ecs.add_component_type<RenderComponent>();
    ecs.add_component_type<TransformComponent>();

    ecs.add_component<RenderComponent>(e);
    ecs.add_component<TransformComponent>(e);

    ecs.get_component<RenderComponent>(e)->mesh_ = car_mesh;
    ecs.get_component<TransformComponent>(e)->position = glm::vec3{ 0.0f, 0.0f, 0.0f };
    ecs.get_component<TransformComponent>(e)->rotation = glm::vec3{ 0.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(e)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };
    ecs.get_component<RenderComponent>(e)->shader = texture_shader;

    render_.initMeshBuffers(ecs);
    // ---

    //Post proccess
    grey_sh.compile(vertex_pp, grey_fs);
    negative_sh.compile(vertex_pp, negative_fs);
    waves_sh.compile(vertex_pp, waves_fs);
    blur_sh.compile(vertex_pp, blur_fs);
    kernel_sh.compile(vertex_pp, kernel_fs);

    postProcc.initData();
    //--

    InitImGui(window->getWindow());

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
        window->toggleFullScreen(input);

        rd.w_width = window->getWidth();
        rd.w_height = window->getHeight();
        postProcc.resize(window->getWidth(), window->getHeight());

        postProcc.beginRender();
        rd.cam.update(input, dt, rd.w_width, rd.w_height);

        skybox.render(rd);
        render_.renderSystem(ecs, rd);

        postProcc.endRender();

        MyImGui();

        if (window->closePressed()) done = true;
    }

    return 0;
}





