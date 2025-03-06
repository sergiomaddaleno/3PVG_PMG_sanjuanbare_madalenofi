
#include "Geometry/quad.h"
#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>


Quad::Quad()
    : VAO(0), VBO(0), EBO(0),
    order_(), data_(),
    size_(0.0f), use_texture_(true),
    texture_path_("../stages/assets/textures/materials/brick_wall/brickwall.jpg"),
    normal_map_path_("../stages/assets/textures/materials/brick_wall/brickwall_normal.jpg"),
    color_({ 0.5f, 0.5f, 0.5f })
{}

Quad::~Quad()
{
}

void Quad::init(const float quad_size) {
    size_ = quad_size;

    data_.clear();
    order_.clear();

    glm::vec3 positions[] = {
        {-size_, size_, 0.0f},
        {size_, size_, 0.0f},
        {size_, -size_, 0.0f},
        {-size_, -size_, 0.0f}
    };

    std::copy(positions, positions + 3, vertex);

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

void Quad::bind()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data_.size() * sizeof(float), data_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, order_.size() * sizeof(int), order_.data(), GL_STATIC_DRAW);

    //---
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //---

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Quad::setTexture(std::string texture_path)
{
    texture_path_ = texture_path;
    texture_.loadTexture(texture_path);

    /*texture_.clear();
    for (auto& loc_text : files_path) {
        Texture tempTexture;
        tempTexture.loadTexture(loc_text);
        texture_.push_back(tempTexture);
    }
    texture_count_ = (int)texture_.size();*/
}

void Quad::setNormalMap(std::string normal_map_path)
{
    normal_map_path_ = normal_map_path;
    normal_map_.loadTexture(normal_map_path_);
}

glm::vec3* Quad::GetVertex()
{
    return vertex;
}

void Quad::draw()
{
    if (use_texture_) {
        texture_.active();
        texture_.bind2D();
    }

    /*int i = 0;
    for (auto& tex : texture_) {
        tex.active(i);
        tex.bind2D();
        i++;
    }*/

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

nlohmann::json Quad::serialize() const
{
    nlohmann::json json_data;

    json_data["size"] = size_;
    json_data["Texture_path"] = texture_path_;

    return json_data;
}




