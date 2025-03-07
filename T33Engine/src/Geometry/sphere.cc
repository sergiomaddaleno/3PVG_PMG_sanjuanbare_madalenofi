
#include "Geometry/sphere.h"
#include "utils.h"

#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>

#include <vector>
#include <iostream>

#define PI 3.14159265359f

Sphere::Sphere()
    : VAO(0), VBO(0), EBO(0),
    num_heights_(0), num_revs_(0), size_(0.0f), 
    num_vertex_(0), use_texture_(true),
    texture_path_("../stages/assets/textures/images/blue.png")
{
}

Sphere::~Sphere()
{
}

void Sphere::init(const float sphere_size, const int num_heights, const int num_revs) {
   
    num_heights_ = num_heights;
    num_revs_ = num_revs;
    size_ = sphere_size;

    vertex_sphere_.clear();
    normals_sphere_.clear();
    uv_sphere_.clear();
    order_.clear();
    data_.clear();

    num_vertex_ = (num_heights + 1) * (num_revs + 1);
    vertex_sphere_.resize(num_vertex_ * 3);
    normals_sphere_.resize(num_vertex_ * 3);
    uv_sphere_.resize(num_vertex_ * 2);

    const float alpha = PI / (float)num_heights;
    const float omega = (PI * 2.0f) / (float)num_revs;

    int index = 0;
   
    for (int i = 0; i <= num_heights; i++) {
        for (int j = 0; j <= num_revs; j++) {
            vertex_sphere_[index * 3] = cosf(omega * j) * cosf(alpha * i - 1.57f) * sphere_size;
            vertex_sphere_[index * 3 + 1] = sinf(alpha * i - 1.57f) * sphere_size;
            vertex_sphere_[index * 3 + 2] = sinf(omega * j) * cosf(alpha * i - 1.57f) * sphere_size;

            normals_sphere_[index * 3] = cosf(omega * j) * cosf(alpha * i - 1.57f);
            normals_sphere_[index * 3 + 1] = sinf(alpha * i - 1.57f);
            normals_sphere_[index * 3 + 2] = sinf(omega * j) * cosf(alpha * i - 1.57f);

            uv_sphere_[index * 2] = 1.0f / (float)num_heights * i;
            uv_sphere_[index * 2 + 1] = 1.0f / (float)num_revs * j;

            index++;
        }
    }
    
    data_.reserve(8 * num_vertex_);
    for (int i = 0; i < num_vertex_; i++) {
        data_.push_back(vertex_sphere_[i * 3]);
        data_.push_back(vertex_sphere_[i * 3 + 1]);
        data_.push_back(vertex_sphere_[i * 3 + 2]);

        data_.push_back(normals_sphere_[i * 3]);
        data_.push_back(normals_sphere_[i * 3 + 1]);
        data_.push_back(normals_sphere_[i * 3 + 2]);

        data_.push_back(uv_sphere_[i * 2]);
        data_.push_back(uv_sphere_[i * 2 + 1]);
    }
    
    order_.reserve((num_heights * num_revs) * 6);
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

void Sphere::bind()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data_.size() * sizeof(float), data_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, order_.size() * sizeof(int), order_.data(), GL_STATIC_DRAW);

    // --- 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // --- 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void Sphere::draw()
{
    if (use_texture_) {
        texture_.active(TextureID::TI_Albedo);
        texture_.bind2D();
    }

    /*int i = 0;
    for (auto& tex : texture_) {
        tex.active(i);
        tex.bind2D();
        i++;
    }*/

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (num_heights_ * num_revs_) * 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Sphere::setTexture(std::string file_path)
{
    texture_path_ = file_path;

    texture_.loadTexture(file_path);

    /*texture_.clear();
    for (auto& loc_text : files_path) {
        Texture tempTexture;
        tempTexture.loadTexture(loc_text);
        texture_.push_back(tempTexture);
    }
    texture_count_ = (int)texture_.size();*/
}

std::vector<float> Sphere::GetVertexSphere()
{
    return vertex_sphere_;
}

nlohmann::json Sphere::serialize() const
{
    nlohmann::json json_data;

    json_data["num_heights"] = num_heights_;
    json_data["num_revs"] = num_revs_;
    json_data["size"] = size_;
    json_data["Texture_path"] = texture_path_;

    return json_data;
}
