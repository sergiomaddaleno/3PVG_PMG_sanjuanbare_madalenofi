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

int main(int, char**) {

    auto window = Window::make(global.window_width, global.window_height, "Atomic Engine", global.full_screen);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();
    //window->setWindowIcon(window->getWindow(), "../assets/textures/images/AE.png");

    Input input{ window->getWindow() };
    // ---

    global.game_data.post_process.initData();

    global.game_data.render_system.initShader("../assets/shaders/entities.vs", "../assets/shaders/entities.fs");
    global.game_data.render_system.setupEntities(global.game_data.ecs);

    global.game_data.save_manager.loadFromFile("../saves/main_scene.json", global.game_data.ecs, global.game_data.render_system, global.game_data.cam, global.game_data.skybox);

    global.game_data.imgui_manager.Init(window->getWindow());
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

        global.game_data.post_process.resize(global.window_width, global.window_height);

        global.game_data.post_process.beginRender();
        global.game_data.cam.update(input, dt, global.window_width, global.window_height);

        if (global.use_skybox) {
            global.game_data.skybox.render(global.game_data.cam);
        }

        global.game_data.render_system.renderEntities(global.game_data.ecs, global.game_data.cam);

        //--

        global.game_data.imgui_manager.Render(window->getWindow());

        if (window->closePressed()) done = true;

    }

    return 0;
}





