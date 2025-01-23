
#include "Geometry/quad.h"
#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>




Quad::Quad()
{


}

Quad::~Quad()
{
}

void Quad::init(const float quad_size) {
    float size = quad_size;

    glm::vec3 positions[] = {
        {-size, size, 0.0f},  
        {size, size, 0.0f},  
        {size, -size, 0.0f}, 
        {-size, -size, 0.0f} 
    };

    glm::vec3 normals[] = {
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}
    };

    glm::vec2 uvs[] = {
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f}
    };

   
    data_.reserve(4 * 8); 
    for (int i = 0; i < 4; ++i) {
        data_.push_back(positions[i].x);
        data_.push_back(positions[i].y);
        data_.push_back(positions[i].z);

        data_.push_back(normals[i].x);
        data_.push_back(normals[i].y);
        data_.push_back(normals[i].z);

        data_.push_back(uvs[i].x);
        data_.push_back(uvs[i].y);
    }

   
    unsigned int order[] = {
        0, 2, 1, 
        2, 0, 3 
    };

    order_.assign(order, order + sizeof(order) / sizeof(order[0]));

    
    
}




