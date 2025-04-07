#include "Geometry/screen_quad.h"

ScreenQuad::ScreenQuad() : size_(1.0f),
VAO(0), VBO(0)
{
}

ScreenQuad::~ScreenQuad()
{
}

void ScreenQuad::initVec2(const float size)
{
    rect_screen_vertices.clear();

    rect_screen_vertices = {
        // positions   // texCoords
        -size, -size,  0.0f, 0.0f,
         size, -size,  1.0f, 0.0f,
         size,  size,  1.0f, 1.0f,
    
        -size, -size,  0.0f, 0.0f,
         size,  size,  1.0f, 1.0f,
        -size,  size,  0.0f, 1.0f
    };
}

void ScreenQuad::bindVec2()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, rect_screen_vertices.size() * sizeof(float), rect_screen_vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void ScreenQuad::drawVec2()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void ScreenQuad::initVec3(const float size)
{
    rect_screen_vertices.clear();

    rect_screen_vertices = {
        // positions          // texCoords
        -size,  size, 0.0f,   0.0f, 1.0f,
        -size, -size, 0.0f,   0.0f, 0.0f,
         size,  size, 0.0f,   1.0f, 1.0f,
         size, -size, 0.0f,   1.0f, 0.0f
    };
}

void ScreenQuad::bindVec3()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, rect_screen_vertices.size() * sizeof(float), rect_screen_vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void ScreenQuad::drawVec3()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
