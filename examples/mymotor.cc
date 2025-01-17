#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"

#include "window.h"
#include "imgui_manager.h"
#include "mesh.h"
#include "render_system.h"
#include "shader.h"
#include "input.h"
#include "post_processing.h"
#include "utils.h"
#include "skybox.h"
#include "ecs_manager.h"
#include "scripting.h"
#include "Components/scripting_component.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <../deps/glm/gtx/matrix_decompose.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> car_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

ImGuiManager imgui_manager;
RenderSystem render_system;
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

//Guizmo
bool useWindow = true;
int gizmoCount = 1;
float camDistance = 8.f;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
static bool useSnap(false);
static float snap[3] = { 1.f, 1.f, 1.f };
int lastUsing = 0;

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

//Guizmo
void RenderScene(const glm::mat4& view, const glm::mat4& projection, int display_w, int display_h) {
    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, (float)display_w, (float)display_h);

    auto& transform = ecs.get_component<TransformComponent>(e);

    glm::vec3 position = transform->position;
    glm::quat rotation = transform->rotation; 
    glm::vec3 scale = transform->scale;

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

    float objectMatrix[16] = {
        modelMatrix[0][0], modelMatrix[0][1], modelMatrix[0][2], modelMatrix[0][3],
        modelMatrix[1][0], modelMatrix[1][1], modelMatrix[1][2], modelMatrix[1][3],
        modelMatrix[2][0], modelMatrix[2][1], modelMatrix[2][2], modelMatrix[2][3],
        modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2], modelMatrix[3][3]
    };

    ImGui::Begin("Gizmo Controls");
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    ImGui::Checkbox("Use Snap", &useSnap);
    if (useSnap) {
        if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE)
            ImGui::InputFloat3("Snap (Translate)", snap);
        else if (mCurrentGizmoOperation == ImGuizmo::ROTATE)
            ImGui::InputFloat("Snap (Rotate)", &snap[0]);
        else if (mCurrentGizmoOperation == ImGuizmo::SCALE)
            ImGui::InputFloat3("Snap (Scale)", snap);
    }
    ImGui::End();

    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
        mCurrentGizmoOperation, ImGuizmo::LOCAL,
        objectMatrix, nullptr,
        useSnap ? snap : nullptr);

    glm::mat4 newModelMatrix = glm::make_mat4(objectMatrix);

    if (mCurrentGizmoOperation == ImGuizmo::TRANSLATE) {
        transform->position = glm::vec3(newModelMatrix[3]);
    }
    else if (mCurrentGizmoOperation == ImGuizmo::ROTATE) {
        glm::mat3 rotationMatrix = glm::mat3(newModelMatrix);

       
        glm::quat newRotationQuat = glm::quat_cast(rotationMatrix);

     
        transform->rotation = newRotationQuat * rotation; 

        
    }
    else if (mCurrentGizmoOperation == ImGuizmo::SCALE) {
        glm::mat3 rotationMatrix = glm::mat3(newModelMatrix);  
        glm::vec3 scaleVec = glm::vec3(
            glm::length(newModelMatrix[0]),  
            glm::length(newModelMatrix[1]),
            glm::length(newModelMatrix[2])
        );

        transform->scale = scaleVec; 
    }

}

void InitImGui(GLFWwindow* w) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void MyImGui(GLFWwindow* w) {

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    glfwGetFramebufferSize(w, &rd.w_width, &rd.w_height);

    glm::mat4 projection = glm::perspective(glm::radians(rd.fov),
        (float)rd.w_width / (float)rd.w_height,
        0.1f, 100.0f);

    ImGui::Begin("Menu");

    ChangeShader();
    ChangePostProcess();

    ImGui::End();

    RenderScene(rd.cam.getViewMatrix(), projection, rd.w_width, rd.w_height);

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
    ecs.get_component<TransformComponent>(e)->rotation = glm::vec3{ 2.0f, 1.0f, 0.0f };
    ecs.get_component<TransformComponent>(e)->scale = glm::vec3{ 0.2f, 0.2f, 0.2f };
    ecs.get_component<RenderComponent>(e)->shader = texture_shader;

    render_system.initMesh(ecs);
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
    //imgui_manager.Init(window->getWindow());

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
        render_system.renderMesh(ecs, rd);

        postProcc.endRender();

        MyImGui(window->getWindow());
        //imgui_manager.Render(window->getWindow(), rd, postProcc);

        if (window->closePressed()) done = true;
    }

    return 0;
}





