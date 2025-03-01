
#ifndef __WAVE__
#define __WAVE__

#include <../deps/glm/glm.hpp>
#include <vector>

class Shader;

class GerstnerWave{

public:
    GerstnerWave();
    ~GerstnerWave();

    void setUniforms(Shader& shader, float time);

    float time_;
    float amplitude_;
    float wave_length_;
    float speed_;
    glm::vec2 direction_;
    bool use_wave_;
};

#endif // __WAVE__