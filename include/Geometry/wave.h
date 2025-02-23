
#ifndef __WAVE__
#define __WAVE__

#include <../deps/glm/glm.hpp>

class GerstnerWave {
public:
    GerstnerWave();

    void update(std::vector<float>& terrain_vertices, float time);

private:
    float amplitude_;
    float wavelength_;
    float frequency_;
    glm::vec3 direction_;
};

#endif // __WAVE__