
#include "../include/Geometry/triangle.h"
#include "Components/render_component.h"

#include "utils.h"

Triangle::Triangle(PointsTriangle t[3]) : VAO(0), VBO(0)
{
    for (int i = 0; i < 3; ++i) {
        my_triangle[i] = t[i];
    }
}

Triangle::~Triangle(){};

Triangle::Triangle(const Triangle& other) : VAO(0), VBO(0)
{
    for (int i = 0; i < 3; ++i) {
        my_triangle[i] = other.my_triangle[i];
    }
}

std::optional<Triangle> Triangle::make(float size_, Color color)
{
    PointsTriangle t[3];

    switch (color) {
    case Color::Red:
        t[0] = { { -0.6f * size_, -0.4f * size_ }, { 1.0f, 0.0f, 0.0f } };
        t[1] = { { 0.6f * size_, -0.4f * size_ }, { 1.0f, 0.0f, 0.0f } };
        t[2] = { { 0.0f,  0.6f * size_ }, { 1.0f, 0.0f, 0.0f } };
        break;

    case Color::Green:
        t[0] = { { -0.6f * size_, -0.4f * size_ }, { 0.0f, 1.0f, 0.0f } };
        t[1] = { { 0.6f * size_, -0.4f * size_ }, { 0.0f, 1.0f, 0.0f } };
        t[2] = { { 0.0f,  0.6f * size_ }, { 0.0f, 1.0f, 0.0f } };
        break;

    case Color::Blue:
        t[0] = { { -0.6f * size_, -0.4f * size_ }, { 0.0f, 0.0f, 1.0f } };
        t[1] = { { 0.6f * size_, -0.4f * size_ }, { 0.0f, 0.0f, 1.0f } };
        t[2] = { { 0.0f,  0.6f * size_ }, { 0.0f, 0.0f, 1.0f } };
        break;

    case Color::Yellow:
        t[0] = { { -0.6f * size_, -0.4f * size_ }, { 1.0f, 1.0f, 0.0f } };
        t[1] = { { 0.6f * size_, -0.4f * size_ }, { 1.0f, 1.0f, 0.0f } };
        t[2] = { { 0.0f,  0.6f * size_ }, { 1.0f, 1.0f, 0.0f } };
        break;

    default:
        t[0] = { { -0.6f * size_, -0.6f * size_ }, { 0.6f, 0.6f, 0.6f } };
        t[1] = { { 0.6f * size_, -0.6f * size_ }, { 0.6f, 0.6f, 0.6f } };
        t[2] = { { 0.6f,  0.6f * size_ }, { 0.6f, 0.6f, 0.6f } };
        break;
    }

    return std::optional<Triangle>(t);
}

void Triangle::bind()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(my_triangle), my_triangle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(PointsTriangle), (void*)offsetof(PointsTriangle, pos_));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PointsTriangle), (void*)offsetof(PointsTriangle, rgb));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Triangle::draw()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}





