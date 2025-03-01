#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#include "Core/window.h"
#include "Core/input.h"
#include "Managers/lua_manager.h"

#include "Geometry/terrain.h"
#include "global_data.h"
#include "utils.h"



void init() {

    global.post_process.initData();
    global.render_system.setup(global.ecs);

    initShader(global.render_system.shader, "../stages/shaders/entities.vs", "../stages/shaders/entities.fs");
    initShader(global.render_system.deferred.gbuffer, "../stages/shaders/g_buffer.vs", "../stages/shaders/g_buffer.fs");
    initShader(global.render_system.deferred.deferred_shading, "../stages/shaders/deferred_shading.vs", "../stages/shaders/deferred_shading.fs");

    global.grid.initGrid();
}

int main(int, char**) {

    auto window = Window::make(global.window_width, global.window_height, "Atomic Engine", global.full_screen);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();
    //window->setWindowIcon(window->getWindow(), "../assets/textures/images/AE.png");

    Input input{ window->getWindow() };

    init();

    global.ui_manager.Init(window->getWindow());

    float dt = 0.0f;
    double currentTime = 0.0f;
    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);

        window->updateWindow();
        window->toggleFullScreen(input);

        if (global.window_width != window->getWidth() ||
            global.window_height != window->getHeight()) {

            global.window_width = window->getWidth();
            global.window_height = window->getHeight();
        }

        global.post_process.resize(global.window_width, global.window_height);
        global.post_process.beginRender();

        global.cam.update(input, dt, global.window_width, global.window_height);

        if (global.use_skybox) global.skybox.render(global.cam);
        global.render_system.render(global.ecs, global.cam);


        global.ui_manager.Render(window->getWindow());

        if (window->closePressed(input)) done = true;
    }

    return 0;
}





