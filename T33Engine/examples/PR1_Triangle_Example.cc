
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

glm::vec2 arrow_dir = { 0.0f, 5.0f };

RenderComponent rc;
TransformComponent tc;
Shader shader;

int main(int, char**) {

    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    initShader(shader, "../stages/shaders/basic/triangle.vs", "../stages/shaders/basic/triangle.fs");

    auto triangle = Triangle::make(1.0f, Triangle::Color::Blue);

    rc.triangle_ = *triangle;
    tc.position = glm::vec3{ arrow_dir.x, arrow_dir.y, 0.0f };
    tc.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
       
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        tc.moveEntity(input, dt, 2.0f);
        rc.triangle_.render(shader, rc, tc);

        if (window->closePressed(input)) done = true;
        lastTime = currentTime;
    }

    return 0;
}




