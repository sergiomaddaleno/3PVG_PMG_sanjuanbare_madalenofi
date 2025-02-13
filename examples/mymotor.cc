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

struct GameData {

};
ImGuiManager imgui_manager;
Camera cam;
PostProcessing postProcc;
ECSManager ecs;
RenderSystem render_system;
Skybox skybox;
SaveManager save_manager;

auto e = ecs.create_entity();

int main(int, char**) {

    auto window = Window::make(g_window_width_, g_window_height_, "Window ", g_full_screen_);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();
    //window->setWindowIcon(window->getWindow(), "../assets/textures/images/AE.png");

    Input input{ window->getWindow() };
    // ---
    
    postProcc.initData();

    render_system.initShader("../assets/shaders/entities.vs", "../assets/shaders/entities.fs");
    render_system.setupEntities(ecs);

    save_manager.loadFromFile("../saves/main_scene.json", ecs, render_system, cam, skybox);

    imgui_manager.Init(window->getWindow());

    data_imgui.ecs = &ecs;
    data_imgui.render_system = &render_system;
    data_imgui.input = &input;
    data_imgui.save_manager = save_manager;
    //--

    double lastTime = window->getTime();
    bool done = false;

    float dt;
    double currentTime;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);

        window->updateWindow();
        window->toggleFullScreen(input);

        g_window_width_ = window->getWidth();
        g_window_height_ = window->getHeight();
        postProcc.resize(g_window_width_, g_window_height_);

        postProcc.beginRender();
        cam.update(input, dt, g_window_width_, g_window_height_);

        if (g_use_skybox_) {
            skybox.render(cam);
        }

        render_system.renderEntities(ecs, cam);

        //--

        data_imgui.cam = &cam;
        data_imgui.post_proccessing = &postProcc;
        data_imgui.window = &window.value();
        data_imgui.skybox = &skybox;
        lastTime = currentTime; 

        imgui_manager.Render(window->getWindow(), data_imgui);

        if (window->closePressed()) done = true;

    }

    return 0;
}





