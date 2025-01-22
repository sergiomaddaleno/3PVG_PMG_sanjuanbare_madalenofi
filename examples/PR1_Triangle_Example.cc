
#include "window.h"
#include "Geometry/triangle.h"
#include "Render/shader.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"
#include "input.h"
#include "utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

glm::vec2 arrow_dir = { 0.0f, 0.0f };

char* vs_tri = (char*)ReadFile("../assets/shaders/triangle.vs");
char* fs_tri = (char*)ReadFile("../assets/shaders/triangle.fs");

RenderComponent rc;
TransformComponent tc;
Shader triangle_shader;

int main(int, char**) {

    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    //--- Triangle 
    triangle_shader.compile(vs_tri, fs_tri);

    auto triangle = Triangle::make(0.5f, Triangle::Color::Blue);

    rc.triangle_ = *triangle;
    rc.shader = triangle_shader;
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

        if (window->closePressed()) done = true;
    }

    return 0;
}




