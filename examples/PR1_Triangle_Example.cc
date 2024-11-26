
#include "../include/triangle.h"
#include "../include/window.h"
#include "..\include\render.h"
#include "..\include\input.h"
#include "..\include\Components\transform_component.h"
#include <../deps/linmath.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <optional>
#include <string>

vec2 arrow_dir = { 0.0f, 0.0f };;
float move_speed = 0.05;

void MoveTriangle(GLFWwindow* w, Input& input) {

    if (input.isKeyPressed(Input::LEFT)) {
        arrow_dir[0] -= move_speed;
    }

    if (input.isKeyPressed(Input::RIGHT)) {
        arrow_dir[0] += move_speed;
    }

    if (input.isKeyPressed(Input::UP)) {
        arrow_dir[1] += move_speed;
    }

    if (input.isKeyPressed(Input::DOWN)) {
        arrow_dir[1] -= move_speed;
    }
}

int main(int, char**) {

    //--- Window
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();
    //---

    //--- Triangle
    auto tri_ = Triangle::make(0.5f, Triangle::Color::Yellow);    

    Render r;
    auto render = r.make();
    ShaderBuffers shaders = r.initShader("../assets/shaders/vs_triangle.vs", "../assets/shaders/fs_triangle.fs", shaders);

    //--- Input
    Input input{ window->GetWindow() };

    TransformComponent tr;

    bool done = false;
    while (!done) {

        //Triangle
        MoveTriangle(window->GetWindow(), input);
        //---
       
        tr.position = Vec3{ arrow_dir[0], arrow_dir[1], 0.0f};
        tr.scale = Vec3{ 2.0f, 2.0f, 2.0f };

        //Render of triangle
        r.SetupShader(shaders);
        r.RenderDrawTriangle(tri_->my_triangle, tr);

        window->UpdateWindow();

        if (window->closePressed()) done = true;
    }

    return 0;
}




