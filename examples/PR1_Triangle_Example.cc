
#include "../include/triangle.h"
#include "../include/window.h"
#include "..\include\render.h"
#include "..\include\input.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../deps/linmath.h>
#include <optional>
#include <string>

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
float move_speed = 0.0005;

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
    glfwInit();
    auto window = Window::make(800, 600, "Window");
    window->SetWindowActive();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glfwMakeContextCurrent(window->GetWindow());
    //---

    //--- Triangle
    auto tri_ = Triangle::make(0.5f, Triangle::Color::Blue);    
    //auto render = Render::make();

    Render r;
    auto render = r.make();
    r.RenderDrawTriangle(vertex_shader_text,fragment_shader_text,tri_->my_triangle);

    //--- Input
    Input input{ window->GetWindow() };

    bool done = false;
    while (!done) {

        glfwGetFramebufferSize(window->GetWindow(), &width, &height);
        float ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        //Triangle
        MoveTriangle(window->GetWindow(), input);

        mat4x4_identity(m);
        mat4x4_translate_in_place(m, arrow_dir.x, arrow_dir.y, 0.0f);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
        //---
       
        //Render of triangle
        glUseProgram(r.GetProgram());
        glUniformMatrix4fv(r.GetLocation(), 1, GL_FALSE, (GLfloat*)mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        window->UpdateWindow();

        if (window->closePressed()) done = true;
    }

    return 0;
}




