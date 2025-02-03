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

std::shared_ptr<Mesh> mario_mesh = std::make_shared<Mesh>();

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

char* vs_entities = (char*)ReadFile("../assets/shaders/entities.vs");
char* fs_entities = (char*)ReadFile("../assets/shaders/entities.fs");

char* vs_sky_box = (char*)ReadFile("../assets/shaders/skybox.vs");
char* fs_sky_box = (char*)ReadFile("../assets/shaders/skybox.fs");

char* dds_file = ReadFile("../assets/textures/skybox/sky.dds");
char* lua_file = ReadFile("../scripts/settings.lua");

Shader light_shader, entities_shader; //entities shader has all

auto e = ecs.create_entity();

int main(int, char**) {

    auto window = Window::make(g_window_width_, g_window_height_, "Window ", g_full_screen_);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();
    //window->setWindowIcon(window->getWindow(), "../assets/textures/images/AE.png");


    Input input{ window->getWindow() };

    // ---

    entities_shader.compile(vs_entities, fs_entities);

    //Skybox
    //if (skyboxTex.loadSkyboxTex(dds_file)) skybox.texture_ = skyboxTex.getTexture();
    //skybox.shader_.compile(vs_sky_box, fs_sky_box);

    //skybox.init();
    // ---
    
    //postProcc.initData();

    render_system.setupEntities(ecs);

    save_manager.loadFromFile("../saves/scene_1.json", ecs, render_system, cam);

    //imgui_manager.Init(window->getWindow());

    //data_imgui.ecs = &ecs;
    //data_imgui.shader = entities_shader;
    //data_imgui.render_system = &render_system;
    //data_imgui.input = &input;
    //--

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
        //postProcc.resize(g_window_width_, g_window_height_);

        //--
        //postProcc.beginRender();
        cam.update(input, dt, g_window_width_, g_window_height_);

        //skybox.render(skybox.shader_, skybox.texture_, cam);

        render_system.renderEntities(ecs, entities_shader ,cam);

        //--

        data_imgui.cam = cam;
        data_imgui.post_proccessing = &postProcc;
        data_imgui.window = &window.value();
        data_imgui.skybox = skybox;

        //imgui_manager.Render(window->getWindow(), data_imgui);

        if (window->closePressed()) done = true;
    }

    return 0;
}





