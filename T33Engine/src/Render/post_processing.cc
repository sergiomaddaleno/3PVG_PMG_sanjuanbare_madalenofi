
#include "Render/post_processing.h"
#include "utils.h"

#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

PostProcessing::PostProcessing()
    : grey_(0.0f), negative_(1.0f), waves_(0.0f), blur_(0.0f),
    kernel_types_(0), frame_buffer_(),
    width_(800), height_(600),
    RBO(0) {
}

PostProcessing::~PostProcessing()
{
}

void PostProcessing::initData()
{
    frame_buffer_.init(width_, height_, true, false);
    frame_buffer_.bind();

    quad_.initVec2();
    quad_.bindVec2();

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Post-processing framebuffer incomplete!" << std::endl;
    }
}

void PostProcessing::beginRender()
{
    frame_buffer_.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::endRender()
{
    frame_buffer_.unbind();
    glDisable(GL_DEPTH_TEST);
}

void PostProcessing::render(Shader& shader)
{

    shader.useProgram();

    // -- 
    GLuint screen_tex_loc = glGetUniformLocation(shader.getProgram(), "u_screen_tex");
    glUniform1i(screen_tex_loc, 0);

    shader.setUniformValue(shader.getUniformLocation("u_grey"), ValueType::T_FLOAT_1, &grey_); 
    shader.setUniformValue(shader.getUniformLocation("u_negative"), ValueType::T_FLOAT_1, &negative_);
    shader.setUniformValue(shader.getUniformLocation("u_waves"), ValueType::T_FLOAT_1, &waves_);
    shader.setUniformValue(shader.getUniformLocation("u_blur"), ValueType::T_FLOAT_1, &blur_);
    shader.setUniformValue(shader.getUniformLocation("u_kernel_types"), ValueType::T_INT_1, &kernel_types_);

    // -- 

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, frame_buffer_.GetColorTexture());

    quad_.drawVec2();

    glBindVertexArray(0);
    glUseProgram(0);
}

void PostProcessing::resize(unsigned int width, unsigned int height)
{
    if (width_ != width || height_ != height) {

        width_ = width;
        height_ = height;

        frame_buffer_.resize(width_, height_);
        this->initData();
    }
}
