
#include "Geometry/cube.h"
#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

Cube::Cube()
    : VAO(0), VBO(0), EBO(0),
    order_(), data_(),
    tex(), size_(0.0f),
    cube_8v_(false), cube_24v_(false) {
}

Cube::~Cube()
{
}

void Cube::init8v(const float cube_size) {

    size_ = cube_size;
    cube_8v_ = true;
    cube_24v_ = false;

    data_.clear();
    order_.clear();
   
    glm::vec3 positions[] = {
        {-size_, -size_, size_}, {-size_, +size_, size_}, {+size_, +size_, size_}, {+size_, -size_, size_},
        {+size_, -size_, -size_}, {+size_, +size_, -size_}, {-size_, +size_, -size_}, {-size_, -size_, -size_},
    };

    glm::vec3 normals[] = {
        {-1.0f, -1.0f, 1.0f}, {-1.0f, +1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f},
        {+1.0f, -1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f}, {-1.0f, +1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f},
    };

    glm::vec2 uvs[] = {
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
    };

    
    data_.reserve(8 * 8); 
    for (int i = 0; i < 8; ++i) {
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
        0, 3, 1, 1, 3, 2,
        3, 4, 2, 2, 4, 5,
        4, 7, 5, 5, 7, 6,
        7, 0, 6, 6, 0, 1,
        1, 2, 6, 6, 2, 5,
        7, 4, 0, 0, 4, 3
    };

    order_.assign(order, order + sizeof(order) / sizeof(order[0]));
   
}

void Cube::init24v(const float cube_size) {
    
    size_ = cube_size;
    cube_8v_ = false;
    cube_24v_ = true;

    data_.clear();
    order_.clear();
    
    glm::vec3 positions[] = {
        
        {-size_, +size_, +size_}, {+size_, +size_, +size_}, {+size_, -size_, +size_}, {-size_, -size_, +size_},
        
        {+size_, +size_, +size_}, {+size_, +size_, -size_}, {+size_, -size_, -size_}, {+size_, -size_, +size_},
       
        {+size_, +size_, -size_}, {-size_, +size_, -size_}, {-size_, -size_, -size_}, {+size_, -size_, -size_},
       
        {-size_, +size_, -size_}, {-size_, +size_, +size_}, {-size_, -size_, +size_}, {-size_, -size_, -size_},
       
        {-size_, +size_, -size_}, {+size_, +size_, -size_}, {+size_, +size_, +size_}, {-size_, +size_, +size_},
       
        {-size_, -size_, +size_}, {+size_, -size_, +size_}, {+size_, -size_, -size_}, {-size_, -size_, -size_},
    };
   
    glm::vec3 normals[] = {
        
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f},
       
        {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
        
        {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
        
        {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
       
        {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
       
        {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
    };
    
    glm::vec2 uvs[] = {
       
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
        
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
        
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
       
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
        
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
       
        {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
    };
   
    data_.reserve(24 * 8);
    for (int i = 0; i < 24; ++i) {
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
      
        0, 2, 1, 2, 0, 3,
      
        4, 6, 5, 6, 4, 7,
        
        8, 10, 9, 10, 8, 11,
        
        12, 14, 13, 14, 12, 15,
       
        16, 18, 17, 18, 16, 19,
       
        20, 22, 21, 22, 20, 23
    };

   
    order_.assign(order, order + sizeof(order) / sizeof(order[0]));
}

void Cube::bind()
{
    glGenVertexArrays(1, &  VAO);
    glGenBuffers(1, &  VBO);
    glGenBuffers(1, &  EBO);

    glBindVertexArray(  VAO);

    glBindBuffer(GL_ARRAY_BUFFER,   VBO);
    glBufferData(GL_ARRAY_BUFFER,   data_.size() * sizeof(float),   data_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,   EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,   order_.size() * sizeof(int),   order_.data(), GL_STATIC_DRAW);

    //--

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //-- 

    if (!  tex.loadTexture("../assets/textures/images/blue.png")) {
        printf("Texture not found\n");
    }
}

void Cube::render()
{
    tex.active();
    tex.bind2D();

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

nlohmann::json Cube::serialize() const
{
    nlohmann::json json_data;

    json_data["size"] = size_;
    json_data["cube_8v"] = cube_8v_;
    json_data["cube_24v"] = cube_24v_;

    return json_data;
}


