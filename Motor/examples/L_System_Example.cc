#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#include "Core/window.h"
#include "Core/input.h"
#include "Managers/lua_manager.h"

#include "IA/l_system.h"
#include "Geometry/cube.h"

#include "global_data.h"
#include "utils.h"

LSystem lsystem;

void init() {

    global.post_process.initData();
    global.render_system.setup(global.ecs);

    global.save_manager.loadFromFile("../stages/saves/LSystem_scene.json");

    initShader(global.render_system.shader, "../stages/shaders/entities.vs", "../stages/shaders/entities.fs");
    initShader(global.render_system.deferred.gbuffer, "../stages/shaders/g_buffer.vs", "../stages/shaders/g_buffer.fs");
    initShader(global.render_system.deferred.deferred_shading, "../stages/shaders/deferred_shading.vs", "../stages/shaders/deferred_shading.fs");

    global.grid.initGrid();

    std::map<char, std::string> rules = {{'F', "FF+[+F-F-F]-[-F+F+F]"}};

    lsystem.init("F", rules, 2);

}

int main(int, char**) {

    auto window = Window::make(global.window_width, global.window_height, "Atomic Engine", global.full_screen);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();
    //window->setWindowIcon(window->getWindow(), "../assets/textures/images/AE.png");

    Input input{ window->getWindow() };

    global.ui_manager.Init(window->getWindow());

    init();

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

        if (global.deferred_shading) {
            global.render_system.deferredRender(global.ecs, global.cam);
        }
        else {
            global.render_system.forwardRender(global.ecs, global.cam);
        }


        global.ui_manager.Render(window->getWindow());

        if (window->closePressed(input)) done = true;
    }

    return 0;
}





