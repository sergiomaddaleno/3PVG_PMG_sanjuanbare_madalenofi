
#include <optional>
#include <string>

#include "Core/window.h"
#include "Geometry/triangle.h"
#include "Core/shader.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"
#include "Core/input.h"
#include "utils.h"

#include <../deps/glm/glm.hpp>

glm::vec2 arrow_dir = { 0.0f, 0.0f };

RenderComponent rc;
TransformComponent tc;

int main(int, char**) {

    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    initShader(rc.triangle_.shader, "../stages/shaders/basic/triangle.vs", "../stages/shaders/basic/triangle.fs");

    auto triangle = Triangle::make(0.5f, Triangle::Color::Blue);

    rc.triangle_ = *triangle;
    tc.position = glm::vec3{ arrow_dir.x, arrow_dir.y, 0.0f };
    tc.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };


    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
       
        tc.moveEntity(input, dt);
        rc.triangle_.render(rc, tc);

        if (window->closePressed(input)) done = true;
    }

    return 0;
}




