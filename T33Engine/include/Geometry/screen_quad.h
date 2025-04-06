/**
 * @file quad.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SCREEN_QUAD__
#define __SCREEN_QUAD__ 1

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
 * @class Quad
 * @brief Represents a 2D screen quad with rendering capabilities.
 */
class ScreenQuad {
public:
    ScreenQuad();
    ~ScreenQuad();

    void initVec2(const float size = 1.0f);
    void bindVec2();
    void drawVec2();

    void initVec3(const float size = 1.0f);
    void bindVec3();
    void drawVec3();
    //---

    float size_;

private:
    GLuint VAO, VBO;

    std::vector<float> rect_screen_vertices;
};

#endif // !__QUAD__