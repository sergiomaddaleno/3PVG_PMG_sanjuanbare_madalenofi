
#include "..\include\triangle.h"
#include "utils.h"

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

std::optional<Triangle> Triangle::make(float size_, Color color)
{
	PointsTriangle t[3];

    switch (color) {
    case Color::Red:
        t[0] = { -0.6f * size_, -0.4f * size_, 1.0f, 0.0f, 0.0f }; 
        t[1] = { 0.6f * size_, -0.4f * size_, 1.0f, 0.0f, 0.0f };  
        t[2] = { 0.0f,  0.6f * size_, 1.0f, 0.0f, 0.0f };
        break;

    case Color::Green:
        t[0] = { -0.6f * size_, -0.4f * size_, 0.0f, 1.0f, 0.0f }; 
        t[1] = { 0.6f * size_, -0.4f * size_, 0.0f, 1.0f, 0.0f };
        t[2] = { 0.0f,  0.6f * size_, 0.0f, 1.0f, 0.0f }; 
        break;

    case Color::Blue:
        t[0] = { -0.6f * size_, -0.4f * size_, 0.0f, 0.0f, 1.0f };
        t[1] = { 0.6f * size_, -0.4f * size_, 0.0f, 0.0f, 1.0f };
        t[2] = { 0.0f,  0.6f * size_, 0.0f, 0.0f, 1.0f };
        break;

    case Color::Yellow:
        t[0] = { -0.6f * size_, -0.4f * size_, 1.0f, 1.0f, 0.0f };
        t[1] = { 0.6f * size_, -0.4f * size_, 1.0f, 1.0f, 0.0f }; 
        t[2] = { 0.0f,  0.6f * size_, 1.0f, 1.0f, 0.0f }; 
        break;

    default:
        t[0] = { -0.6f * size_, -0.6f * size_, 0.6f, 0.6f, 0.6f };
        t[1] = { 0.6f * size_, -0.6f * size_, 0.6f, 0.6f, 0.6f };
        t[2] = { 0.6f,  0.6f * size_, 0.6f, 0.6f, 0.6f };
        break;
    }

	return std::optional<Triangle>( t );
}

void Triangle::render(RenderData& rd)
{
    GLuint vertex_buffer;
    GLint vpos_location, vcol_location;

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rd.render_comp.triangle_.my_triangle[0]) * 3, rd.render_comp.triangle_.my_triangle, GL_STATIC_DRAW);

    vpos_location = glGetAttribLocation(rd.render_comp.shader.getProgram(), "vPos");
    vcol_location = glGetAttribLocation(rd.render_comp.shader.getProgram(), "vCol");

    rd.render_comp.shader.useProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(rd.transf_comp.position.x, rd.transf_comp.position.y, rd.transf_comp.position.z));
    model = glm::rotate(model, rd.transf_comp.angle, glm::vec3(rd.transf_comp.rotation.x, rd.transf_comp.rotation.y, rd.transf_comp.rotation.z));
    model = glm::scale(model, glm::vec3(rd.transf_comp.scale.x, rd.transf_comp.scale.y, rd.transf_comp.scale.z));

    glm::mat4 view = rd.cam.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(rd.fov), (float)rd.w_width / rd.w_height, 0.1f, 100.0f);
    glm::mat4 mvp = projection * view * model;

    GLint mvpLocation = glGetUniformLocation(rd.render_comp.shader.getProgram(), "MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));

    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(vpos_location);

    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(vcol_location);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);

    glUseProgram(0);
}

