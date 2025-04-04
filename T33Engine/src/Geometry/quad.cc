
#include "Geometry/quad.h"
#include "utils.h"

#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>


Quad::Quad()
    : VAO(0), VBO(0), EBO(0),
    order_(), data_(),
    size_(0.0f), parallax_scale_(0.08f),
    use_texture_(true), use_normal_map_(false), use_parallax_map_(false),
    texture_path_("../stages/assets/textures/images/blue.png"),
    normal_map_path_(""), parallax_map_path_(""),
    parallax_type(ParallaxType::ParallaxMapping)
{
    std::fill_n(vertex, 3, glm::vec3(0.0f));
}

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
        {0.0f, 0.0f, 1.0f}, 
        {0.0f, 0.0f, 1.0f}, 
        {0.0f, 0.0f, 1.0f}, 
        {0.0f, 0.0f, 1.0f}
    };

    glm::vec2 uvs[] = {
        {0.0f, 1.0f}, 
        {1.0f, 1.0f}, 
        {1.0f, 0.0f},
        {0.0f, 0.0f}
    };

    //-------------------------------------
    //For normal mapping
    
    //First triangle
    glm::vec3 edge1 = positions[1] - positions[0];
    glm::vec3 edge2 = positions[2] - positions[0];
    glm::vec2 deltaUV1 = uvs[1] - uvs[0];
    glm::vec2 deltaUV2 = uvs[2] - uvs[0];

    //Apply Formula
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent1, bitangent1;
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    //---

    //Second triangle
    edge1 = positions[2] - positions[0];
    edge2 = positions[3] - positions[0];
    deltaUV1 = uvs[2] - uvs[0];
    deltaUV2 = uvs[3] - uvs[0];

    //Apply Formula
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent2, bitangent2;
    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    //tangent2 = glm::normalize(tangent2);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    //bitangent2 = glm::normalize(bitangent2);
    //-------------------------------------

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

        //---

        if (i == 0 || i == 1 || i == 2) {
            data_.push_back(tangent1.x);
            data_.push_back(tangent1.y);
            data_.push_back(tangent1.z);

            data_.push_back(bitangent1.x);
            data_.push_back(bitangent1.y);
            data_.push_back(bitangent1.z);
        }
        else {
            data_.push_back(tangent2.x);
            data_.push_back(tangent2.y);
            data_.push_back(tangent2.z);

            data_.push_back(bitangent2.x);
            data_.push_back(bitangent2.y);
            data_.push_back(bitangent2.z);
        }
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    //---
    
    //For normal mapping
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Quad::setTexture(std::string texture_path)
{
    texture_path_ = texture_path;
    texture_.loadTexture(texture_path);
}

void Quad::setNormalMap(std::string normal_map_path)
{
    normal_map_path_ = normal_map_path;
    normal_map_.loadTexture(normal_map_path_);
}

void Quad::setParallaxMap(std::string parallax_map_path)
{
    parallax_map_path_ = parallax_map_path;
    parallax_map_.loadTexture(parallax_map_path_);
}

glm::vec3* Quad::GetVertex()
{
    return vertex;
}

void Quad::draw()
{
    if (use_texture_) {
        texture_.active(TextureID::TI_Diffuse);
        texture_.bind2D();
    }

    if (use_normal_map_) {
        normal_map_.active(TextureID::TI_Normal);
        normal_map_.bind2D();
    }
    
    if (use_parallax_map_) {
        parallax_map_.active(TextureID::TI_Parallax);
        parallax_map_.bind2D();
    }

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
    json_data["NormalMap_path"] = normal_map_path_;
    json_data["ParallaxMap_path"] = parallax_map_path_;

    return json_data;
}




