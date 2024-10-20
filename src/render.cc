
#include "..\include\render.h"
#include "..\include\mesh.h"

#include <../deps/linmath.h>

Render::Render() : VAO(0), VBO(0), EBO(0) {}

std::optional<Render> Render::make()
{  
    glewInit();
   
    return std::optional<Render>();
}

GLuint Render::GetProgram()
{
    return program_;
}

GLint Render::GetLocation()
{
    return mvp_location_;
}

void Render::RenderDrawTriangle(static const char* vertex_shader_text, static const char* fragment_shader_text,
    Triangle::PointsTriangle(&vertices)[3])
{
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint vpos_location, vcol_location, mvp_location;

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

    program_ = program;
    mvp_location_ = mvp_location;

}

void Render::RenderMesh()
{
    Mesh m;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), m.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(unsigned int), m.indices.data(), GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}


