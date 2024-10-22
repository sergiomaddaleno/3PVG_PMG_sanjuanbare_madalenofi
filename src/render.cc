
#include "..\include\render.h"
#include "..\include\mesh.h"
#define M_PI 3.14159265358979323846
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

void Render::RenderMesh(static const char* vertex_shader_text, static const char* fragment_shader_text, Mesh& m)
{

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- Usar el programa de shaders ---
    glUseProgram(shaderProgram);

    // --- Limpieza de buffers ---
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glEnable(GL_DEPTH_TEST);

    // --- Configurar matrices ---
    mat4x4 model, view, projection;
    mat4x4_identity(model);   

    float angle = (float)glfwGetTime();  
    mat4x4_rotate(model, model, 0.0f, 1.0f, 0.0f, angle);
    mat4x4_identity(view);
    mat4x4_translate(view, 0.0f, 0.0f, -3.0f); 

    mat4x4_perspective(projection, (float)(55.0f * (M_PI / 180.0f)), 800.0f / 600.0f, 0.1f, 100.0f);  

    // Enviar las matrices al shader
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (const GLfloat*)model);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (const GLfloat*)view);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (const GLfloat*)projection);

    vec3 lightPos = { 1.0f, 1.0f, 1.0f };
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, (const GLfloat*)&lightPos);

    vec3 viewPos = { 1.0f, 1.0f, 3.0f }; 
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, (const GLfloat*)&viewPos);

    // --- Configurar los buffers (VAO, VBO, EBO) ---
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), &m.vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(unsigned int), &m.indices[0], GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    // Normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // Coordenadas de textura 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    glEnableVertexAttribArray(2);

    // --- Dibujar la malla ---
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // --- Limpiar el estado ---
    glUseProgram(0);

}




