
#include "Render/post_processing.h"
#include "utils.h"

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

PostProcessing::PostProcessing()
{
}

PostProcessing::~PostProcessing()
{
}

void PostProcessing::initData()
{
    frame_buffer_.init(width_, height_, true, true);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    // --

    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);
    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect_screen_vertices), rect_screen_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void PostProcessing::beginRender()
{
    frame_buffer_.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::endRender()
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer_.GetFBO());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    
    glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    frame_buffer_.unbind();
}

void PostProcessing::render(Shader& shader, Effect effect)
{
    shader.useProgram();

    // -- 
    GLuint screen_tex_loc = glGetUniformLocation(shader.getProgram(), "u_screen_tex");
    glUniform1i(screen_tex_loc, 0);

    switch (effect) {
    case Effect::Grey: shader.setUniformValue(shader.getUniformLocation("u_grey"), Type::T_FLOAT_1, &grey_); break;

    case Effect::Negative: shader.setUniformValue(shader.getUniformLocation("u_negative"), Type::T_FLOAT_1, &negative_); break;

    case Effect::Waves: shader.setUniformValue(shader.getUniformLocation("u_waves"), Type::T_FLOAT_1, &waves_); break;

    case Effect::Blur: shader.setUniformValue(shader.getUniformLocation("u_blur"), Type::T_FLOAT_1, &blur_); break;

    case Effect::Kernel: shader.setUniformValue(shader.getUniformLocation("u_kernel_types"), Type::T_INT_1, &kernel_types_); break;
    
    }
    // -- 

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame_buffer_.GetColorTexture());

    //Screen quad
    glBindVertexArray(rectVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glUseProgram(0);

}

void PostProcessing::resize(unsigned int width, unsigned int height)
{
    width_ = width;
    height_ = height;

    frame_buffer_.resize(width_, height_);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);
}
