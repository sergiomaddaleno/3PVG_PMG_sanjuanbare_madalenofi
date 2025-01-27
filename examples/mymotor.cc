#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Core/window.h"
#include "Core/framebuffer.h"
#include "Core/shader.h"
#include "Core/input.h"
#include "Core/mesh.h"
#include "Render/render_system.h"
#include "Render/camera.h"
#include "Render/post_processing.h"
#include "Render/skybox.h"
#include "Managers/imgui_manager.h"
#include "Managers/save_manager.h"
#include "Managers/lua_manager.h"

#include "Systems/ecs_manager.h"
#include "Geometry/terrain.h"
#include "global_variables.h"
#include "utils.h"

#include "Components/scripting_component.h"
#include "Components/light_component.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <../deps/glm/gtx/matrix_decompose.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#define M_PI 3.14159265358979323846

std::shared_ptr<Mesh> car_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();
std::shared_ptr<Mesh> light_bulb_mesh = std::make_shared<Mesh>();

ImGuiManager imgui_manager;
FrameBuffer fb;
RenderSystem render_system;
Camera cam;
Skybox skybox;
Texture skyboxTex;
Terrain terrain;
ECSManager ecs;
PostProcessing postProcc;
RenderComponent render_comp;
TransformComponent transf_comp;
SaveManager save_manager;

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

/*
void ChangeShader() {

    const char* shaders_name[]{ "Texture Shader", "Normals shader" };
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

    const char* pp_name[]{ "None", "Grey", "Negative", "Waves", "Blur", "Kernel types" };
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
*/

int main(int, char**) {

    auto window = Window::make(g_window_width_, g_window_height_, "Window ", g_full_screen_);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();

    Input input{ window->getWindow() };

    // ---

    if (!mario_mesh->loadOBJ("../assets/obj/Mario.obj", "../assets/textures/mario")) {
        return -1;
    }

    texture_shader.compile(vs_texture, fs_texture);
    normals_shader.compile(vs_normals, fs_normals);
    light_shader.compile(vs_simple, fs_light);
    entities_shader.compile(vs_entities, fs_entities);

    //Skybox
    if (skyboxTex.loadSkyboxTex(dds_file)) {
        render_comp.texture = skyboxTex.getTexture();
    }

    render_comp.shader.compile(vs_sky_box, fs_sky_box);

    skybox.init();
    // ---

    //ECS
    render_system.setupEntities(ecs);
    
    render_system.createDirectionalLight(ecs, entities_shader);
   
    render_system.createMesh(ecs, mario_mesh, entities_shader);

    render_system.createTerrain(ecs, entities_shader, 300, 300, 4.0f, true, true);

    render_system.initEntities(ecs);

    // ---
    //Post proccess
    grey_sh.compile(vertex_pp, grey_fs);
    negative_sh.compile(vertex_pp, negative_fs);
    waves_sh.compile(vertex_pp, waves_fs);
    blur_sh.compile(vertex_pp, blur_fs);
    kernel_sh.compile(vertex_pp, kernel_fs);

    postProcc.initData();
    //--

    imgui_manager.Init(window->getWindow());
    data_imgui.ecs = &ecs;
    data_imgui.shader = &entities_shader;
    data_imgui.render_system = &render_system;
    data_imgui.input = &input;


    double lastTime = window->getTime();
    bool done = false;

    float dt;
    double currentTime;
    while (!done) {
        currentTime = window->getTime();
        dt = float(currentTime - lastTime);
        window->updateWindow();
        window->toggleFullScreen(input);

        g_window_width_ = window->getWidth();
        g_window_height_ = window->getHeight();
        postProcc.resize(g_window_width_, g_window_height_);

        //--
        postProcc.beginRender();
        cam.update(input, dt, g_window_width_, g_window_height_);
        lastTime = currentTime;

        skybox.render(render_comp, cam);

        render_system.renderEntities(ecs, cam);

        postProcc.endRender();
        //--

        data_imgui.cam = cam;
        data_imgui.post_proccessing = postProcc;
        data_imgui.game_texture = postProcc.frame_buffer_.GetColorTexture();
        data_imgui.window = &window.value();

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        imgui_manager.Render(window->getWindow(), data_imgui);
       

        if (window->closePressed()) done = true;
    }

    return 0;
}





