#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#include "Core/window.h"
#include "Core/input.h"
#include "Managers/lua_manager.h"

#include "global_data.h"
#include "utils.h"

std::vector<std::string> faces
{
    "../assets/textures/skybox/cubemap/Tantolunden/posx.jpg", // Right
    "../assets/textures/skybox/cubemap/Tantolunden/negx.jpg", // Left
    "../assets/textures/skybox/cubemap/Tantolunden/posy.jpg", // Top
    "../assets/textures/skybox/cubemap/Tantolunden/negy.jpg", // Bottom
    "../assets/textures/skybox/cubemap/Tantolunden/posz.jpg", // Front
    "../assets/textures/skybox/cubemap/Tantolunden/negz.jpg"  // Back
};

int main(int, char**) {

    auto window = Window::make(global.window_width, global.window_height, "Atomic Engine", global.full_screen);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();
    //window->setWindowIcon(window->getWindow(), "../assets/textures/images/AE.png");

    Input input{ window->getWindow() };
    // ---

    global.post_process.initData();

    global.render_system.initShader("../assets/shaders/entities.vs", "../assets/shaders/entities.fs");
    global.render_system.setupEntities(global.ecs);

    global.save_manager.loadFromFile("../saves/main_scene.json", global.ecs, global.render_system, global.cam, global.skybox);

    //global.skybox.loadCubeMap(faces, "../assets/shaders/skybox_cubemap.vs", "../assets/shaders/skybox_cubemap.fs");
    //global.skybox.init();

    global.imgui_manager.Init(window->getWindow());
    //--

    double lastTime = window->getTime();
    bool done = false;

    float dt = 0.0f;
    double currentTime = 0.0f;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);

        window->updateWindow();
        window->toggleFullScreen(input);

        global.window_width = window->getWidth();
        global.window_height = window->getHeight();

        global.post_process.resize(global.window_width, global.window_height);

        global.post_process.beginRender();
        global.cam.update(input, dt, global.window_width, global.window_height);

        if (global.use_skybox) {
            global.skybox.renderDDS(global.cam);
        }

        global.skybox.renderCubeMap(global.cam);

        global.render_system.renderEntities(global.ecs, global.cam);

        //--

        global.imgui_manager.Render(window->getWindow());

        if (window->closePressed()) done = true;

    }

    return 0;
}





