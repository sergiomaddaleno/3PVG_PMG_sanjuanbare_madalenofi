/**
 * @file wave.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __WAVE__
#define __WAVE__

#include <../deps/glm/glm.hpp>
#include <vector>

#include "Core/shader.h"

/**
 * @class GerstnerWave
 * @brief Implements the Gerstner wave algorithm for realistic water surface simulation.
 */
class GerstnerWave{

public:
    GerstnerWave();
    ~GerstnerWave();

    std::vector<float> waveEffect(const std::vector<float>& aPos, float time);
    
    void setUniforms(Shader& shader, float time) {
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

    float time_;
    float amplitude_;
    float wave_length_;
    float speed_;
    glm::vec2 direction_;
    bool use_wave_;
};

#endif // !__WAVE__