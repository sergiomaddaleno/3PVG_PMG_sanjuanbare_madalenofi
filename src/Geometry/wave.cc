
#include "Geometry/wave.h"
#include <math.h>

#define PI 3.14159265359f
#define PI_2 PI * 2.0f

GerstnerWave::GerstnerWave()
    : amplitude_(2.0f), wavelength_(1.0f), frequency_(1.0f), direction_(glm::vec3(1.0f, 0.0f, 0.0f)) {}


void GerstnerWave::update(std::vector<float>& terrain_vertices, float time)
{
    int vertexCount = terrain_vertices.size() / 3;

    float k = PI_2 / wavelength_;
    float omega = PI_2 * frequency_;

    for (int i = 0; i < vertexCount; i++) {
        float x = terrain_vertices[i * 8 + 0];
        float z = terrain_vertices[i * 8 + 2];

        float wave = amplitude_ * sinf(k * (x * direction_.x + z * direction_.y) + omega * time);

        terrain_vertices[i * 8 + 1] = wave;
    }
}





