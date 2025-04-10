/**
 * @file frameBuffer.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class FrameBuffer
  * @brief A class for managing OpenGL frame buffers.
  */
class FrameBuffer {
public:
    FrameBuffer();
    ~FrameBuffer();

    void init(int width, int height,
        bool useColor = true,
        bool useDepth = true);

    void bind();
    void unbind();
    void resize(int width, int height);

    GLuint captureWindowToTexture();

    GLuint GetFBO() const;
    GLuint GetColorTexture() const;
    GLuint GetDepthTexture() const;

    int width_, height_;
private:

    GLuint FBO_;
    GLuint FBColorTexture_;
    GLuint FBDepthTexture_;

    bool use_color_;
    bool use_depth_;
};

#endif // !__FRAMEBUFFER_H__