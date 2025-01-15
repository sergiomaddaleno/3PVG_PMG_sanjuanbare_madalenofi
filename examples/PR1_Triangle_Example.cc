
#include "window.h"
#include "triangle.h"
#include "input.h"
#include "shader.h"
#include "utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

glm::vec2 arrow_dir = { 0.0f, 0.0f };

char* vs_tri = (char*)ReadFile("../assets/shaders/vs_triangle.vs");
char* fs_tri = (char*)ReadFile("../assets/shaders/fs_triangle.fs");

Shader triangle_shader;
RenderData rd;

int main(int, char**) {

    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    window->initGLEW();

    Input input{ window->getWindow() };

    //--- Triangle 
    triangle_shader.compile(vs_tri, fs_tri);

    auto triangle = Triangle::make(0.5f, Triangle::Color::Blue);

    rd.render_comp.triangle_ = *triangle;
    rd.transf_comp.position = glm::vec3{ arrow_dir.x, arrow_dir.y, 0.0f };
    rd.transf_comp.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };
    rd.render_comp.shader = triangle_shader;

    double lastTime = window->getTime();
    bool done = false;
    while (!done) {
        double currentTime = window->getTime();
        float dt = float(currentTime - lastTime);
        window->updateWindow();
       
        rd.transf_comp.moveEntity(input, dt);
        rd.render_comp.triangle_.render(rd);

        if (window->closePressed()) done = true;
    }

    return 0;
}




