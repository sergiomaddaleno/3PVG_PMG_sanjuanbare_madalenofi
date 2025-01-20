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
#include "Render/mesh.h"
#include "Render/render_system.h"
#include "Render/shader.h"
#include "input.h"
#include "Render/post_processing.h"
#include "utils.h"
#include "Render/skybox.h"
#include "Systems/ecs_manager.h"
#include "Systems/scripting.h"
#include "Components/scripting_component.h"
#include "Geometry/terrain.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <../deps/glm/gtx/matrix_decompose.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include "Components/light_component.h"

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> car_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> light_bulb_mesh = std::make_shared<Mesh>();

ImGuiManager imgui_manager;
RenderSystem render_system;
RenderData rd;
LuaScript ls;
Skybox skybox;
Texture skyboxTex;
Terrain terrain;
ECSManager ecs;
PostProcessing postProcc;

auto e = ecs.create_entity();
int selected_entity = -1;

char* vs_normals = (char*)ReadFile("../assets/shaders/normals.vs");
char* fs_normals = (char*)ReadFile("../assets/shaders/normals.fs");

char* vs_texture = (char*)ReadFile("../assets/shaders/texture.vs");
char* fs_texture = (char*)ReadFile("../assets/shaders/texture.fs");

char* vs_entities = (char*)ReadFile("../assets/shaders/entities.vs");
char* fs_entities = (char*)ReadFile("../assets/shaders/entities.fs");

char* fs_light = (char*)ReadFile("../assets/shaders/light.fs");
char* vs_simple = (char*)ReadFile("../assets/shaders/simple_vertex.vs");

char* vs_sky_box = (char*)ReadFile("../assets/shaders/skybox.vs");
char* fs_sky_box = (char*)ReadFile("../assets/shaders/skybox.fs");

char* vertex_pp = (char*)ReadFile("../assets/shaders/postprocess/vertex_pp.vs");
char* grey_fs = (char*)ReadFile("../assets/shaders/postprocess/grey.fs");
char* negative_fs = (char*)ReadFile("../assets/shaders/postprocess/negative.fs");
char* waves_fs = (char*)ReadFile("../assets/shaders/postprocess/waves.fs");
char* blur_fs = (char*)ReadFile("../assets/shaders/postprocess/blur.fs");
char* kernel_fs = (char*)ReadFile("../assets/shaders/postprocess/kernel.fs");

char* dds_file = ReadFile("../assets/textures/skybox/sky.dds");
char* lua_file = ReadFile("../scripts/settings.lua");

Shader normals_shader, texture_shader, light_shader, entities_shader; //entities shader has all
Shader grey_sh, negative_sh, waves_sh, blur_sh, kernel_sh;

//Guizmo
bool useWindow = true;
int gizmoCount = 1;
float camDistance = 8.f;
static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
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

void RenderScene(const glm::mat4& view, const glm::mat4& projection, int display_w, int display_h, int selected_entity) {
    static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    static bool useSnap = false;
    static float snap[3] = { 1.0f, 1.0f, 1.0f };

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, (float)display_w, (float)display_h);

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();

    int entity_id = 0;

    for (; ri != ecs.getComponentList<RenderComponent>().end() &&
        ti != ecs.getComponentList<TransformComponent>().end();
        ++ri, ++ti, ++entity_id) {
        if (entity_id == selected_entity && ri->has_value() && ti->has_value()) {

            auto& transform = ecs.get_component<TransformComponent>(selected_entity);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform->position);
            model = model * glm::mat4_cast(transform->rotation);
            model = glm::scale(model, transform->scale);

            float objectMatrix[16];
            std::memcpy(objectMatrix, glm::value_ptr(model), sizeof(objectMatrix));

            ImGuizmo::SetRect(0, 0, (float)display_w, (float)display_h);

            ImGui::Begin("Gizmo Controls", nullptr,
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoTitleBar);
            if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
                mCurrentGizmoOperation = ImGuizmo::SCALE;

            ImGui::End();

            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                mCurrentGizmoOperation, ImGuizmo::LOCAL,
                objectMatrix, nullptr);

            if (ImGuizmo::IsUsing()) {
                glm::mat4 newModelMatrix = glm::make_mat4(objectMatrix);
                glm::vec3 scale, translation, skew;
                glm::vec4 perspective;
                glm::quat rotation;

                // Decompose the matrix back into position, rotation, and scale
                glm::decompose(newModelMatrix, scale, rotation, translation, skew, perspective);

                transform->position = translation;
                transform->rotation = rotation;
                transform->scale = scale;
            }

            break;
        }
    }
}

void InitImGui(GLFWwindow* w) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(w, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void SelectorEntitys(GLFWwindow* w) {
    ImGui::Begin("Entity Selector");

    auto ri = ecs.getComponentList<RenderComponent>().begin();
    auto ti = ecs.getComponentList<TransformComponent>().begin();
    auto riend = ecs.getComponentList<RenderComponent>().end();
    auto tiend = ecs.getComponentList<TransformComponent>().end();

    int entity_id = 0;

    for (; ri != riend && ti != tiend; ++ri, ++ti, ++entity_id) {
        if (ti->has_value()) {
            if (ImGui::Selectable(("Entity " + std::to_string(entity_id)).c_str(), selected_entity == entity_id)) {
                selected_entity = entity_id;
            }
        }
    }

    glm::mat4 projection = glm::perspective(glm::radians(rd.fov),
        (float)rd.w_width / (float)rd.w_height,
        0.1f, 100.0f);

    RenderScene(rd.cam.getViewMatrix(), projection, rd.w_width, rd.w_height, selected_entity);

    ImGui::End();
}

void MyImGui(GLFWwindow* w) {

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    glfwGetFramebufferSize(w, &rd.w_width, &rd.w_height);

    ImGui::Begin("Menu");

    ChangeShader();
    ChangePostProcess();
    SelectorEntitys(w);
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

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    texture_shader.compile(vs_texture, fs_texture);
    normals_shader.compile(vs_normals, fs_normals);
    light_shader.compile(vs_simple, fs_light);
    entities_shader.compile(vs_entities, fs_entities);

    //Skybox
    if (skyboxTex.loadSkyboxTex(dds_file)) {
        rd.render_comp.skybox_tex = skyboxTex.getTexture();
    }

    rd.render_comp.skybox_shader.compile(vs_sky_box, fs_sky_box);

    skybox.init();
    // ---

    //ECS
    render_system.setupEntities(ecs);

    render_system.createMesh(ecs, mario_mesh, entities_shader);

    render_system.createSpotLight(ecs, entities_shader);
    render_system.createPointLight(ecs, entities_shader);
    //render_system.createTerrain(ecs, terrain, entities_shader);

    render_system.initEntities(ecs);

    //Terrain
    //terrain.init();

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

        render_system.renderEntities(ecs, rd);
       
        postProcc.endRender();

        MyImGui(window->getWindow());
        //imgui_manager.Render(window->getWindow(), rd, postProcc);

        if (window->closePressed()) done = true;
    }

    return 0;
}





