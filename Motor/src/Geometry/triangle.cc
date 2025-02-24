
#include "../include/Geometry/triangle.h"
#include "Components/render_component.h"
#include "Components/transform_component.h"
#include "utils.h"

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

Triangle::Triangle(PointsTriangle t[3])
{
    for (int i = 0; i < 3; ++i) {
        my_triangle[i] = t[i];
    }
}

Triangle::~Triangle(){};

Triangle::Triangle(const Triangle& other)
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

void Triangle::render(RenderComponent rc, TransformComponent tc, Camera cam)
{
    /*GLuint vertex_buffer;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rc.triangle_.my_triangle[0]) * 3, rc.triangle_.my_triangle, GL_STATIC_DRAW);

    rc.shader.useProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, tc.position);
    model = model * glm::mat4_cast(tc.rotation);
    model = glm::scale(model, tc.scale);

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getProjectionMatrix();
    glm::mat4 mvp = projection * view * model;

    GLint mvpLocation = rc.shader.getUniformLocation("mvp");
    rc.shader.setUniformValue(mvpLocation, ValueType::T_MAT_4x4, glm::value_ptr(mvp));

    GLuint vpos_location = glGetAttribLocation(rc.shader.getProgram(), "a_position");
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vpos_location);

    GLuint vcol_location = glGetAttribLocation(rc.shader.getProgram(), "a_color");
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(vcol_location);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);

    glUseProgram(0);*/
}



