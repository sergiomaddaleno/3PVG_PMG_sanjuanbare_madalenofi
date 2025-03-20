#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>

#include "Core/window.h"
#include "Core/input.h"

#include "global_data.h"
#include "utils.h"

void init() {

    global.post_process.initData();
    global.render_system.setup();

    global.save_manager.loadFromFile("../stages/saves/solar_system.json");

    initShader(global.render_system.shader, "../stages/shaders/forward_shading.vs", "../stages/shaders/forward_shading.fs");
    initShader(global.render_system.deferred.gbuffer, "../stages/shaders/g_buffer.vs", "../stages/shaders/g_buffer.fs");
    initShader(global.render_system.deferred.deferred_shading, "../stages/shaders/deferred_shading.vs", "../stages/shaders/deferred_shading.fs");

    global.grid.initGrid();
}

int main(int, char**) {

    auto window = Window::make(global.window_width, global.window_height, "T33Engine", global.full_screen);
    window->setWindowActive();
    window->initGLEW();
    window->setResizeCallback();

    Input input{ window->getWindow() };

    global.ui_manager.Init(&window.value(), &input);

    init();

    double dt = 0.0;
    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        double dt = currentTime - lastTime;
        global.dt = dt;

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
        
        if (global.deferred_shading){
            global.render_system.deferredRender(global.cam);
        }else{
            global.render_system.forwardRender(global.cam);
        }

        /*global.ecs.get_component<TransformComponent>(3)->position = 
            global.ecs.get_component<TransformComponent>(3)->position 
            + glm::vec3(cosf(glfwGetTime()), 0.0f, sinf(glfwGetTime()));*/

        global.ui_manager.Render();

        if (window->closePressed(input)) done = true;
        lastTime = currentTime;
    }

    return 0;
}





