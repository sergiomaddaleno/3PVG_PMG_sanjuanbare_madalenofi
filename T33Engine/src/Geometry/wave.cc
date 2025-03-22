
#include "Geometry/wave.h"
#include "Core/shader.h"
#include "Components/render_component.h"


GerstnerWave::GerstnerWave() : time_(0.0f), amplitude_(0.2f),
wave_length_(2.0f), speed_(1.0f), direction_({ 1.0f, 0.5f }), use_wave_(false)
{
}

GerstnerWave::~GerstnerWave()
{
}

std::vector<float> GerstnerWave::waveEffect(const std::vector<float>& aPos, float time) {
    if (!use_wave_) return aPos;

    std::vector<float> modifiedPos = aPos; 

    float k = 6.28318530718f / wave_length_;
    float omega = k * speed_;
    glm::vec2 dir = glm::normalize(direction_);

    for (size_t i = 0; i < aPos.size() / 3; i++) {
        float x = aPos[i * 3 + 0];
        float y = aPos[i * 3 + 1];
        float z = aPos[i * 3 + 2];

        float wave = amplitude_ * sin(k * glm::dot(glm::vec2(x, z), dir) + omega * time);

        modifiedPos[i * 3 + 1] = y + wave; 
    }

    return modifiedPos;
}

void GerstnerWave::setUniforms(Shader& shader, float time)
{
	int usewave = use_wave_;
	shader.setUniformValue(shader.getUniformLocation("u_use_wave"), ValueType::T_INT_1, &usewave);

	if (use_wave_) {
		shader.setUniformValue(shader.getUniformLocation("u_time"), ValueType::T_FLOAT_1, &time);
		shader.setUniformValue(shader.getUniformLocation("u_wave_amplitude"), ValueType::T_FLOAT_1, &amplitude_);
		shader.setUniformValue(shader.getUniformLocation("u_wave_length"), ValueType::T_FLOAT_1, &wave_length_);
		shader.setUniformValue(shader.getUniformLocation("u_wave_speed"), ValueType::T_FLOAT_1, &speed_);
		shader.setUniformValue(shader.getUniformLocation("u_wave_direction"), ValueType::T_FLOAT_2, &direction_);
	}
}
