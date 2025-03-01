
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

void GerstnerWave::setUniforms(Shader& shader, float time)
{
	if (use_wave_) {
		shader.setUniformValue(shader.getUniformLocation("u_use_wave"), ValueType::T_FLOAT_1, &use_wave_);
		shader.setUniformValue(shader.getUniformLocation("u_time"), ValueType::T_FLOAT_1, &time);
		shader.setUniformValue(shader.getUniformLocation("u_wave_amplitude"), ValueType::T_FLOAT_1, &amplitude_);
		shader.setUniformValue(shader.getUniformLocation("u_wave_length"), ValueType::T_FLOAT_1, &wave_length_);
		shader.setUniformValue(shader.getUniformLocation("u_wave_speed"), ValueType::T_FLOAT_1, &speed_);
		shader.setUniformValue(shader.getUniformLocation("u_wave_direction"), ValueType::T_FLOAT_2, &direction_);
	}
}
