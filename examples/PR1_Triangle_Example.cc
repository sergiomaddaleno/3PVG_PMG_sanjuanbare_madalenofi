
#include "../include/triangle.h"
#include "../include/window.h"
#include "..\include\render.h"
#include "..\include\input.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../deps/linmath.h>
#include <optional>
#include <string>

#include "..\include\Components\transform_component.h"

struct Vec2 {
    float x, y;
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

int width, height;
mat4x4 m, p, mvp;

Vec2 arrow_dir;
float move_speed = 0.05;

void MoveTriangle(GLFWwindow* w, Input &input) {

    if (input.isKeyPressed(Input::LEFT)) {
        arrow_dir.x -= move_speed;
    }

    if (input.isKeyPressed(Input::RIGHT)) {
        arrow_dir.x += move_speed;
    }

    if (input.isKeyPressed(Input::UP)) {
        arrow_dir.y += move_speed;
    }

    if (input.isKeyPressed(Input::DOWN)) {
        arrow_dir.y -= move_speed;
    }
}

int main(int, char**) {

    //--- Window
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();
    //---

    //--- Triangle
    auto tri_ = Triangle::make(0.5f, Triangle::Color::Blue);    

    Render r;
    auto render = r.make();

    //--- Input
    Input input{ window->GetWindow() };

    TransformComponent tr;

    bool done = false;
    while (!done) {

        glfwGetFramebufferSize(window->GetWindow(), &width, &height);
        float ratio = width / (float)height;

        //Triangle
        MoveTriangle(window->GetWindow(), input);
        //---
       
        tr.position = Vec3{ arrow_dir.x, arrow_dir.y, 0.0f };
        tr.scale = Vec3{ 2.0f, 2.0f, 2.0f };

        //Render of triangle
        r.SetupShader(vertex_shader_text, fragment_shader_text);
        r.RenderDrawTriangle(tri_->my_triangle, tr);

        window->UpdateWindow();

        if (window->closePressed()) done = true;
    }

    return 0;
}




