#include <cmath>
#include <vector>
#include <algorithm>
#include <optional>
#include <string>
#include "Managers/engine.h"
#include "Core/window.h"
#include "Core/input.h"
#include "utils.h"

#include "imgui.h"

int main(int, char**) {

    auto window = Window::make(800, 600, "T33Engine", true);
    Input input{ window->getWindow() };

    Engine T33Engine{*window, input};

    T33Engine.init();

    T33Engine.save_manager.loadFromFile("../stages/saves/main_scene.json");

    bool done = false;
    while (!done) {
        T33Engine.beginFrame();

        /*T33Engine.ui_manager.AddCustomWindow([]() {
            ImGui::Begin("New window");

            ImGui::End();
        });*/

        T33Engine.endFrame();
        if (window->closePressed(input)) done = true;
    }

    return 0;
}





