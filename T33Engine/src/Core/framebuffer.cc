#include "Core/framebuffer.h"
#include <cstdio>

FrameBuffer::FrameBuffer()
    : FBO_(0), FBColorTexture_(0), FBDepthTexture_(0), width_(800), height_(600), use_color_(true), use_depth_(true)
{}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::init(int width, int height, bool use_color, bool use_depth)
{
    width_ = width;
    height_ = height;
    use_color_ = use_color;
    use_depth_ = use_depth;

    glGenFramebuffers(1, &FBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

    if (use_color_) {
        glGenTextures(1, &FBColorTexture_);
        glBindTexture(GL_TEXTURE_2D, FBColorTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBColorTexture_, 0);
    }

    if (use_depth_) {
        glGenTextures(1, &FBDepthTexture_);
        glBindTexture(GL_TEXTURE_2D, FBDepthTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, FBDepthTexture_, 0);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Error: Framebuffer is not complete\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
}

void FrameBuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(int width, int height)
{
    width_ = width;
    height_ = height;

    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

    if (use_color_) {
        glBindTexture(GL_TEXTURE_2D, FBColorTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    if (use_depth_) {
        glBindTexture(GL_TEXTURE_2D, FBDepthTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBuffer::captureWindowToTexture() {
    GLuint captureTexture;
    glGenTextures(1, &captureTexture);

    glBindTexture(GL_TEXTURE_2D, captureTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width_, height_, 0);

    return captureTexture;
}

GLuint FrameBuffer::GetFBO() const
{
    return FBO_;
}

GLuint FrameBuffer::GetColorTexture() const
{
    return FBColorTexture_;
}

GLuint FrameBuffer::GetDepthTexture() const
{
    return FBDepthTexture_;
}