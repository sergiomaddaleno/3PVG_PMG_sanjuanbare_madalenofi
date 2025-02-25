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

    global.imgui_manager.Init(window->getWindow());

    float dt = 0.0f;
    double currentTime = 0.0f;
    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);

        window->updateWindow();
        window->toggleFullScreen(input);

        global.window_width = window->getWidth();
        global.window_height = window->getHeight();

        global.cam.update(input, dt, global.window_width, global.window_height);

        global.imgui_manager.Render(window->getWindow());

        if (window->closePressed(input)) done = true;
    }

    return 0;
}





