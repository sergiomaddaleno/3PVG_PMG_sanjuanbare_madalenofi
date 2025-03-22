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

class Shader;

/**
 * @class GerstnerWave
 * @brief Implements the Gerstner wave algorithm for realistic water surface simulation.
 */
class GerstnerWave{

public:
    GerstnerWave();
    ~GerstnerWave();

    std::vector<float> waveEffect(const std::vector<float>& aPos, float time);
    void setUniforms(Shader& shader, float time);

    float time_;
    float amplitude_;
    float wave_length_;
    float speed_;
    glm::vec2 direction_;
    bool use_wave_;
};

#endif // !__WAVE__