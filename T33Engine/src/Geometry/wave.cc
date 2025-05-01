
#include "Geometry/wave.h"
#include "Components/render_component.h"


GerstnerWave::GerstnerWave() : time_(0.0f), amplitude_(0.2f),
wave_length_(2.0f), speed_(1.0f), direction_({ 1.0f, 0.5f }), use_wave_(false)
{
}

GerstnerWave::~GerstnerWave()
{
}

float GerstnerWave::waveEffect(float x, float z) {
    float k = 6.28318530718f / wave_length_;
    float omega = k * speed_;
    glm::vec2 dir = glm::normalize(direction_);
    float wave = amplitude_ * sin(k * glm::dot(glm::vec2(x, z), dir) + omega * (float)glfwGetTime());
    return wave;
}
