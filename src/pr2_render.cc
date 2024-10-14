#include "..\include\tija\pr2_render.h"

#include <tija/linmath.h>


Render::Render(GLuint program, GLint mvp_location)
    : program_(program), mvp_location_(mvp_location) {}

std::optional<Render> Render::make(static const char* vertex_shader_text, static const char* fragment_shader_text, Triangle::PointsTriangle* vertices)
{
    

    GLuint vertex_buffer, vertex_shader, fragment_shader,program;
    GLint vpos_location, vcol_location, mvp_location;

    glewInit();

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
   
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));

    return std::optional<Render>(Render(program, mvp_location));
}

GLuint Render::GetProgram()
{
    return program_;
}

GLint Render::GetLocation()
{
    return mvp_location_;
}


