
#include "Geometry/sphere.h"
#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#define PI 3.14159265359


Sphere::Sphere()
{

    num_heights_=0;
    num_revs_=0;
}

Sphere::~Sphere()
{
}

void Sphere::init(bool sky, const float sphere_size, const int num_heights, const int num_revs) {
   
    num_heights_ = num_heights;
    num_revs_ = num_revs;
    size_ = sphere_size;
    sky_ = sky;

    int num_vertex = (num_heights + 1) * (num_revs + 1);
    std::vector<float> vertex_sphere(num_vertex * 3); 
    std::vector<float> normals_sphere(num_vertex * 3);
    std::vector<float> uv_sphere(num_vertex * 2);

    const float alpha = PI / num_heights;
    const float omega = (PI * 2.0f) / num_revs;

    int index = 0;

   
    for (int i = 0; i <= num_heights; i++) {
        for (int j = 0; j <= num_revs; j++) {
            vertex_sphere[index * 3] = cosf(omega * j) * cosf(alpha * i - 1.57f) * sphere_size;
            vertex_sphere[index * 3 + 1] = sinf(alpha * i - 1.57f) * sphere_size;
            vertex_sphere[index * 3 + 2] = sinf(omega * j) * cosf(alpha * i - 1.57f) * sphere_size;

            normals_sphere[index * 3] = cosf(omega * j) * cosf(alpha * i - 1.57f);
            normals_sphere[index * 3 + 1] = sinf(alpha * i - 1.57f);
            normals_sphere[index * 3 + 2] = sinf(omega * j) * cosf(alpha * i - 1.57f);

            uv_sphere[index * 2] = 1.0f / (float)num_heights * i;
            uv_sphere[index * 2 + 1] = 1.0f / (float)num_revs * j;

            index++;
        }
    }

    
    data_.reserve(8 * num_vertex);
    for (int i = 0; i < num_vertex; i++) {
        data_.push_back(vertex_sphere[i * 3]);
        data_.push_back(vertex_sphere[i * 3 + 1]);
        data_.push_back(vertex_sphere[i * 3 + 2]);

        data_.push_back(normals_sphere[i * 3]);
        data_.push_back(normals_sphere[i * 3 + 1]);
        data_.push_back(normals_sphere[i * 3 + 2]);

        data_.push_back(uv_sphere[i * 2]);
        data_.push_back(uv_sphere[i * 2 + 1]);
    }

    
    order_.reserve((num_heights * num_revs) * 6);
    index = 0;
    if (!sky) {
        for (int i = 0; i < num_heights; i++) {
            for (int j = 0; j < num_revs; j++) {
                int base_index = ((num_heights + 1) * i) + j;
                order_.push_back(base_index);
                order_.push_back(base_index + num_heights + 1);
                order_.push_back(base_index + 1);
                order_.push_back(base_index + 1);
                order_.push_back(base_index + num_heights + 1);
                order_.push_back(base_index + num_heights + 2);
            }
        }
    }
    else {
        for (int i = 0; i < num_heights; i++) {
            for (int j = 0; j < num_revs; j++) {
                int base_index = ((num_heights + 1) * i) + j;
                order_.push_back(base_index + num_heights + 1);
                order_.push_back(base_index);
                order_.push_back(base_index + 1);
                order_.push_back(base_index + num_heights + 1);
                order_.push_back(base_index + 1);
                order_.push_back(base_index + num_heights + 2);
            }
        }
    }
   
}


float* Sphere::GetVertexSphere()
{
    return nullptr;
}
