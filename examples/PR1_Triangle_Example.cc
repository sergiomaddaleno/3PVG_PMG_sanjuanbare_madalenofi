
#include "../include/triangle.h"
#include "../include/window.h"
#include "..\include\render.h"
#include "..\include\input.h"
#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

glm::vec2 arrow_dir = { 0.0f, 0.0f };
float move_speed = 0.04;

int main(int, char**) {

    //--- Window
    auto window = Window::make(800, 600, "Window");
    window->setWindowActive();
    //---

    //--- Triangle
    auto tri_ = Triangle::make(0.5f, Triangle::Color::Blue);    

    Render r;
    auto render = r.make();
    ShaderBuffers shaders = r.initShader("../assets/shaders/vs_triangle.vs", "../assets/shaders/fs_triangle.fs", shaders);

    //--- Input
    Input input{ window->getWindow() };

    TransformComponent tr;
    tr.position = glm::vec3{ arrow_dir.x, arrow_dir.y, 0.0f };
    tr.scale = glm::vec3{ 2.0f, 2.0f, 2.0f };

    RenderComponent rc;
    rc.triangle_ = std::make_shared<Triangle>(*tri_);;

    bool done = false;
    while (!done) {
       
        tr.moveEntity(input, move_speed);

        //Render of triangle
        r.setupShader(shaders);
        r.renderGeometry(rc, tr);

        window->updateWindow();

        if (window->closePressed()) done = true;
    }

    return 0;
}




