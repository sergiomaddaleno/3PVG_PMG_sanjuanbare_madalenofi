
#include "Geometry/terrain.h"
#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include "../deps/stb_image.h"
#include <string>
#include <iostream>


Terrain::Terrain() {
        num_rows_ = 0;
        num_cols_ = 0;

}
Terrain::~Terrain() {}

void Terrain::init(const int num_cols, const int num_rows,
    const float height_mult, const float size,
    const bool is_centered, const bool use_perlin_noise) {

        num_cols_ = num_cols;
        num_rows_ = num_rows;
        float height_mult_ = height_mult;
        float size_ = size;
        bool is_centered_ = is_centered;
        bool use_perlin_noise_ = use_perlin_noise;

        int num_vertex = (num_cols + 1) * (num_rows + 1);
        std::vector<float> vertex_terrain(num_vertex * 3); // x, y, z
        std::vector<float> normals_terrain(num_vertex * 3); // nx, ny, nz
        std::vector<float> uv_terrain(num_vertex * 2); // u, v

        int index = 0;

        if (!use_perlin_noise) {
            height_map.loadTexture("../assets/textures/heightmaps/heightmap.png");
        }

        glm::vec3 center = { 0.0f, 0.0f, 0.0f };
        if (is_centered) {
            center.x = num_cols * size / 2.0f;
            center.z = num_rows * size / 2.0f;
        }

        // Vértices
        for (int i = 0; i <= num_cols; i++) {
            for (int j = 0; j <= num_rows; j++) {
                float height_value = 0.0f;

                if (use_perlin_noise) {
                    float perlin_x = (float)j;
                    float perlin_z = (float)i;
                    height_value = SimplexNoise::noise(perlin_x, perlin_z);
                }
                else {
                    if (height_map.getData() && i < height_map.getwidth() && j < height_map.getHeight()) {
                        int mapped_i = glm::clamp((int)((float)i / num_cols * height_map.getHeight()), 0, height_map.getHeight() - 1);
                        int mapped_j = glm::clamp((int)((float)j / num_rows * height_map.getwidth()), 0, height_map.getwidth() - 1);

                        unsigned char* texel = height_map.getData() + (mapped_j + height_map.getwidth() * mapped_i) * height_map.getChannels();
                        unsigned char y = height_map.getChannels() >= 1 ? texel[0] : 0;

                        float yScale = 32.0f / 256.0f;
                        float yShift = 10.0f;

                        height_value = ((int)y * yScale - yShift) * height_mult;
                    }
                }

                float x = is_centered ? (j * size) - center.x : j * size;
                float z = is_centered ? (i * size) - center.z : i * size;

                vertex_terrain[index * 3] = x;
                vertex_terrain[index * 3 + 1] = height_value;
                vertex_terrain[index * 3 + 2] = z;

                index++;
            }
        }

        index = 0;
        //Normales
        for (int i = 0; i <= num_cols; i++) {
            for (int j = 0; j <= num_rows; j++) {
                glm::vec3 vertex = { vertex_terrain[index * 3], vertex_terrain[index * 3 + 1], vertex_terrain[index * 3 + 2] };
                glm::vec3 vertexLeft, vertexRight, vertexUp, vertexDown;

                if (i > 0) {
                    vertexLeft = { vertex_terrain[index * 3 - ((num_rows + 1) * 3)] - vertex.x,
                                   vertex_terrain[index * 3 - ((num_rows + 1) * 3) + 1] - vertex.y,
                                   vertex_terrain[index * 3 - ((num_rows + 1) * 3) + 2] - vertex.z };
                }
                else {
                    vertexLeft = { 0.0f, 0.0f, 0.0f };
                }

                if (i != num_cols) {
                    vertexRight = { vertex_terrain[index * 3 + ((num_rows + 1) * 3)] - vertex.x,
                                    vertex_terrain[index * 3 + ((num_rows + 1) * 3) + 1] - vertex.y,
                                    vertex_terrain[index * 3 + ((num_rows + 1) * 3) + 2] - vertex.z };
                }
                else {
                    vertexRight = { 0.0f, 0.0f, 0.0f };
                }

                if (j != num_rows) {
                    vertexUp = { vertex_terrain[index * 3 + 3] - vertex.x,
                                 vertex_terrain[index * 3 + 3 + 1] - vertex.y,
                                 vertex_terrain[index * 3 + 3 + 2] - vertex.z };
                }
                else {
                    vertexUp = { 0.0f, 0.0f, 0.0f };
                }

                if (j > 0) {
                    vertexDown = { vertex_terrain[index * 3 - 3] - vertex.x,
                                   vertex_terrain[index * 3 - 3 + 1] - vertex.y,
                                   vertex_terrain[index * 3 - 3 + 2] - vertex.z };
                }
                else {
                    vertexDown = { 0.0f, 0.0f, 0.0f };
                }

                glm::vec3 CrossRight = glm::cross(vertexRight, vertexUp);
                glm::vec3 CrossUp = glm::cross(vertexUp, vertexLeft);
                glm::vec3 CrossLeft = glm::cross(vertexLeft, vertexDown);
                glm::vec3 CrossDown = glm::cross(vertexDown, vertexRight);

                glm::vec3 normal = glm::normalize(CrossRight + CrossUp + CrossLeft + CrossDown);

                normals_terrain[index * 3] = normal.x;
                normals_terrain[index * 3 + 1] = normal.y;
                normals_terrain[index * 3 + 2] = normal.z;

                index++;
            }
        }

        index = 0;
        for (int i = 0; i <= num_cols; i++) {
            for (int j = 0; j <= num_rows; j++) {
                uv_terrain[index * 2] = 1.0f / (float)(num_cols)*j;
                uv_terrain[index * 2 + 1] = 1.0f / (float)(num_rows)*i;
                index++;
            }
        }
        
        data_.reserve(8 * num_vertex);
        for (int i = 0; i < num_vertex; i++) {
            data_.push_back(vertex_terrain[i * 3]);
            data_.push_back(vertex_terrain[i * 3 + 1]);
            data_.push_back(vertex_terrain[i * 3 + 2]);

            data_.push_back(normals_terrain[i * 3]);
            data_.push_back(normals_terrain[i * 3 + 1]);
            data_.push_back(normals_terrain[i * 3 + 2]);

            data_.push_back(uv_terrain[i * 2]);
            data_.push_back(uv_terrain[i * 2 + 1]);
        }

        order_.reserve((num_cols * num_rows) * 6);
        index = 0;
        for (int i = 0; i < num_cols; i++) {
            for (int j = 0; j < num_rows; j++) {
                int base_index = ((num_rows + 1) * i) + j;
                order_.push_back(base_index);
                order_.push_back(base_index + num_rows + 1);
                order_.push_back(base_index + 1);
                order_.push_back(base_index + 1);
                order_.push_back(base_index + num_rows + 1);
                order_.push_back(base_index + num_rows + 2);
            }
        }    
}

float* Terrain::GetVertexTerrain()
{
    return nullptr;
}



 