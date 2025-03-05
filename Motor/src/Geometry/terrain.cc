
#include "Geometry/terrain.h"
#include "Core/shader.h"
#include "../deps/stb_image.h"
#include "../Perlin/SimplexNoise.h"
#include "utils.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>
#include <../deps/glm/gtc/quaternion.hpp>
#include "../deps/stb_image.h"
#include <string>
#include <iostream>
#include <math.h>

#define GLSL(x) "#version 330\n"#x
static const char* gridVertex = GLSL(
    layout(location = 0) in vec3 aPos;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    uniform vec3 u_camera_pos;
    uniform float u_grid_scale;
    
    void main() {
        vec3 pos = aPos;
        vec3 camera_offset = u_camera_pos / u_grid_scale;

        //pos.xz += camera_offset.xz - fract(u_camera_pos.xz);
        pos.xz += floor(camera_offset.xz);

        gl_Position = projection * view * model * vec4(pos, 1.0);
    }
);

static const char* gridFragment = GLSL(
    out vec4 FragColor;
    uniform vec3 u_grid_color;
    
    void main() {
        
        FragColor = vec4(u_grid_color, 1.0f);
    }
);

Terrain::Terrain()
    : VAO(0), VBO(0), EBO(0),
    num_rows_(0), num_cols_(0), num_vertex_(0),
    height_mult_(0.0f), size_(0.0f),
    is_centered_(true), use_perlin_noise_(true),
    use_texture_(true), 
    texture_path_("../stages/assets/textures/images/grass.png"),
    heightmap_path_("../stages/assets/textures/heightmaps/heightmap.png"),
    grid_color_({ 0.5f, 0.5f, 0.5f}), grid_scale_(20.0f),
    color_({ 0.5f, 0.5f, 0.5f })
{
}

Terrain::~Terrain() {}

void Terrain::init(const int num_cols, const int num_rows,
    const float height_mult, const float size,
    const bool is_centered, const bool use_perlin_noise) {

        num_cols_ = num_cols;
        num_rows_ = num_rows;
        height_mult_ = height_mult;
        size_ = size;
        is_centered_ = is_centered;
        use_perlin_noise_ = use_perlin_noise;

        vertex_terrain_.clear();
        normals_terrain_.clear();
        uv_terrain_.clear();
        order_.clear();
        data_.clear();

        num_vertex_ = (num_cols + 1) * (num_rows + 1);
        vertex_terrain_.resize(num_vertex_ * 3); 
        normals_terrain_.resize(num_vertex_ * 3);
        uv_terrain_.resize(num_vertex_ * 2);

        int index = 0;

        if (!use_perlin_noise) {
            setHeightMap(heightmap_path_);
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
                    height_value = SimplexNoise::noise(perlin_x, perlin_z) * height_mult;
                }
                else {
                    if (height_map_.getData() && i < height_map_.getwidth() && j < height_map_.getHeight()) {
                        int mapped_i = glm::clamp((int)((float)i / num_cols * height_map_.getHeight()), 0, height_map_.getHeight() - 1);
                        int mapped_j = glm::clamp((int)((float)j / num_rows * height_map_.getwidth()), 0, height_map_.getwidth() - 1);

                        unsigned char* texel = height_map_.getData() + (mapped_j + height_map_.getwidth() * mapped_i) * height_map_.getChannels();
                        unsigned char y = height_map_.getChannels() >= 1 ? texel[0] : 0;

                        float yScale = 32.0f / 256.0f;
                        float yShift = 10.0f;

                        height_value = ((int)y * yScale - yShift) * height_mult;
                    }
                }

                float x = is_centered ? (j * size) - center.x : j * size;
                float z = is_centered ? (i * size) - center.z : i * size;

                vertex_terrain_[index * 3] = x;
                vertex_terrain_[index * 3 + 1] = height_value;
                vertex_terrain_[index * 3 + 2] = z;

                index++;
            }
        }

        index = 0;
        //Normales
        for (int i = 0; i <= num_cols; i++) {
            for (int j = 0; j <= num_rows; j++) {
                glm::vec3 vertex = { vertex_terrain_[index * 3], vertex_terrain_[index * 3 + 1], vertex_terrain_[index * 3 + 2] };
                glm::vec3 vertexLeft, vertexRight, vertexUp, vertexDown;

                if (i > 0) {
                    vertexLeft = { vertex_terrain_[index * 3 - ((num_rows + 1) * 3)] - vertex.x,
                                   vertex_terrain_[index * 3 - ((num_rows + 1) * 3) + 1] - vertex.y,
                                   vertex_terrain_[index * 3 - ((num_rows + 1) * 3) + 2] - vertex.z };
                }
                else {
                    vertexLeft = { 0.0f, 0.0f, 0.0f };
                }

                if (i != num_cols) {
                    vertexRight = { vertex_terrain_[index * 3 + ((num_rows + 1) * 3)] - vertex.x,
                                    vertex_terrain_[index * 3 + ((num_rows + 1) * 3) + 1] - vertex.y,
                                    vertex_terrain_[index * 3 + ((num_rows + 1) * 3) + 2] - vertex.z };
                }
                else {
                    vertexRight = { 0.0f, 0.0f, 0.0f };
                }

                if (j != num_rows) {
                    vertexUp = { vertex_terrain_[index * 3 + 3] - vertex.x,
                                 vertex_terrain_[index * 3 + 3 + 1] - vertex.y,
                                 vertex_terrain_[index * 3 + 3 + 2] - vertex.z };
                }
                else {
                    vertexUp = { 0.0f, 0.0f, 0.0f };
                }

                if (j > 0) {
                    vertexDown = { vertex_terrain_[index * 3 - 3] - vertex.x,
                                   vertex_terrain_[index * 3 - 3 + 1] - vertex.y,
                                   vertex_terrain_[index * 3 - 3 + 2] - vertex.z };
                }
                else {
                    vertexDown = { 0.0f, 0.0f, 0.0f };
                }

                glm::vec3 CrossRight = glm::cross(vertexRight, vertexUp);
                glm::vec3 CrossUp = glm::cross(vertexUp, vertexLeft);
                glm::vec3 CrossLeft = glm::cross(vertexLeft, vertexDown);
                glm::vec3 CrossDown = glm::cross(vertexDown, vertexRight);

                glm::vec3 normal = glm::normalize(CrossRight + CrossUp + CrossLeft + CrossDown);

                normals_terrain_[index * 3] = normal.x;
                normals_terrain_[index * 3 + 1] = normal.y;
                normals_terrain_[index * 3 + 2] = normal.z;

                index++;
            }
        }

        index = 0;
        for (int i = 0; i <= num_cols; i++) {
            for (int j = 0; j <= num_rows; j++) {
                uv_terrain_[index * 2] = 1.0f / (float)(num_cols)*j;
                uv_terrain_[index * 2 + 1] = 1.0f / (float)(num_rows)*i;
                index++;
            }
        }
        
        data_.reserve(8 * num_vertex_);
        for (int i = 0; i < num_vertex_; i++) {
            data_.push_back(vertex_terrain_[i * 3]);
            data_.push_back(vertex_terrain_[i * 3 + 1]);
            data_.push_back(vertex_terrain_[i * 3 + 2]);

            data_.push_back(normals_terrain_[i * 3]);
            data_.push_back(normals_terrain_[i * 3 + 1]);
            data_.push_back(normals_terrain_[i * 3 + 2]);

            data_.push_back(uv_terrain_[i * 2]);
            data_.push_back(uv_terrain_[i * 2 + 1]);
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

void Terrain::bind()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data_.size() * sizeof(float), data_.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, order_.size() * sizeof(int), order_.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Terrain::setTexture(std::string file_path)
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

void Terrain::setHeightMap(std::string file_path) {

    heightmap_path_ = file_path;

    height_map_.loadTexture(file_path);
}

void Terrain::draw(){

    if(use_texture_){
        texture_.active();
        texture_.bind2D();
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (num_cols_ * num_rows_) * 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::vector<float> Terrain::GetVertexTerrain()
{
    return vertex_terrain_;
}

void Terrain::initGrid(const int num_cols, const int num_rows)
{
    init(num_cols, num_rows, 0.0f, 1.0f, true, true);

    bind();
    gridShader.compile(gridVertex, gridFragment);
}

void Terrain::drawGrid(Camera cam)
{
    gridShader.useProgram();

    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = cam.getUsePerspectiveMatrix() ? cam.getPerspectiveMatrix() : cam.getOrthograficMatrix();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(grid_scale_, 1.0f, grid_scale_));

    gridShader.setUniformValue(gridShader.getUniformLocation("model"), ValueType::T_MAT_4x4, glm::value_ptr(model));
    gridShader.setUniformValue(gridShader.getUniformLocation("view"), ValueType::T_MAT_4x4, glm::value_ptr(view));
    gridShader.setUniformValue(gridShader.getUniformLocation("projection"), ValueType::T_MAT_4x4, glm::value_ptr(projection));

    glm::vec3 cameraPos = cam.getPosition();
    gridShader.setUniformValue(gridShader.getUniformLocation("u_camera_pos"), ValueType::T_FLOAT_3, glm::value_ptr(cameraPos));
    gridShader.setUniformValue(gridShader.getUniformLocation("u_grid_color"), ValueType::T_FLOAT_3, &grid_color_);
    gridShader.setUniformValue(gridShader.getUniformLocation("u_grid_scale"), ValueType::T_FLOAT_1, &grid_scale_);

    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);

    GLint prevPolygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, prevPolygonMode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (num_cols_ * num_rows_) * 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT, prevPolygonMode[0]);
    glPolygonMode(GL_BACK, prevPolygonMode[1]);

    glDepthMask(GL_TRUE);
    glUseProgram(0);
}


nlohmann::json Terrain::serialize() const
{
    nlohmann::json json_data;

    json_data["num_rows"] = num_rows_;
    json_data["num_cols"] = num_cols_;
    json_data["height_mult"] = height_mult_;
    json_data["size"] = size_;
    json_data["is_centered"] = is_centered_;
    json_data["use_perlin_noise"] = use_perlin_noise_;
    json_data["Texture_path"] = texture_path_;
    json_data["Heightmap_path"] = heightmap_path_;

    return json_data;
}



 