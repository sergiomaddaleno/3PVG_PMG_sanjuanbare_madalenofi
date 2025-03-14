
#include "Geometry/surface.h"

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

Surface::Surface() : VAO(0), VBO(0), EBO(0),
num_heights_(0), num_revs_(0), surface_points_(),
num_vertex_(0), use_texture_(true),
texture_path_("../stages/assets/textures/images/blue.png")
{
}

Surface::~Surface()
{
}

void Surface::init(std::vector<float> surface_points, const int num_heights, const int num_revs)
{
	num_heights_ = num_heights;
	num_revs_ = num_revs;
	surface_points_ = surface_points;

	vertex_.clear();
	normals_.clear();
	uv_.clear();
	order_.clear();
	data_.clear();

	num_vertex_ = (num_heights + 1) * (num_revs + 1);
	vertex_.resize(num_vertex_ * 3);
	normals_.resize(num_vertex_ * 3);
	uv_.resize(num_vertex_ * 2);

	int index = 0;

	float* points = surface_points.data();

	// Calculate vertices
	const float alpha_step = (2 * PI) / num_heights;
	const float omega_step = (2 * PI) / num_revs;

	for (int i = 0; i <= num_heights; ++i) {
		float alpha = (2 * PI / num_heights) * i;

		for (int j = 0; j <= num_revs; ++j) {

			vertex_[index++] = points[3 * j] * cosf(alpha) + points[3 * j + 2] * sinf(alpha);
			vertex_[index++] = points[3 * j + 1];
			vertex_[index++] = (-points[3 * j]) * sinf(alpha) + points[3 * j + 2] * cosf(alpha);
		}
	}

	index = 0;

	// Calculate uvs
	for (int i = 0; i <= num_heights; i++) {
		for (int j = 0; j <= num_revs; j++) {
			uv_[index * 2] = 1.0f / (float)(num_heights)*j;
			uv_[index * 2 + 1] = 1.0f / (float)(num_revs)*i;
			index++;
		}
	}

	index = 0;

	// Calculate normals
	for (int i = 0; i <= num_heights; ++i) {
		float alpha = (2 * PI / num_heights) * i;

		for (int j = 0; j <= num_revs; ++j) {
			float x = points[3 * j];
			float y = points[3 * j + 1];
			float z = points[3 * j + 2];

			float nx = x * cosf(alpha) - z * sinf(alpha);
			float ny = y;
			float nz = x * sinf(alpha) + z * cosf(alpha);

			float normal_length = sqrt(nx * nx + ny * ny + nz * nz);
			if (normal_length > 0.0f) {
				nx /= normal_length;
				ny /= normal_length;
				nz /= normal_length;
			}

			normals_[index * 3] = -nz;
			normals_[index * 3 + 1] = ny;
			normals_[index * 3 + 2] = -nx;

			index++;
		}
	}

	data_.reserve(8 * num_vertex_);
	for (int i = 0; i < num_vertex_; i++) {
		data_.push_back(vertex_[i * 3]);
		data_.push_back(vertex_[i * 3 + 1]);
		data_.push_back(vertex_[i * 3 + 2]);

		data_.push_back(normals_[i * 3]);
		data_.push_back(normals_[i * 3 + 1]);
		data_.push_back(normals_[i * 3 + 2]);

		data_.push_back(uv_[i * 2]);
		data_.push_back(uv_[i * 2 + 1]);
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

void Surface::bind()
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
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// --- 

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Surface::draw()
{
	if (use_texture_) {
		texture_.active(TextureID::TI_Diffuse);
		texture_.bind2D();
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (num_heights_ * num_revs_) * 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Surface::setTexture(std::string file_path)
{
	texture_path_ = file_path;

	texture_.loadTexture(file_path);
}

std::vector<float> Surface::getVertex()
{
	return std::vector<float>();
}

nlohmann::json Surface::serialize() const
{
	nlohmann::json json_data;

	nlohmann::json points_array = nlohmann::json::array();
	for (size_t i = 0; i < surface_points_.size(); i += 3) {
		points_array.push_back({
			surface_points_[i],
			surface_points_[i + 1],
			surface_points_[i + 2]
			});
	}

	json_data["num_heights"] = num_heights_;
	json_data["num_revs"] = num_revs_;
	json_data["Points"] = points_array;
	json_data["Texture_path"] = texture_path_;

	return json_data;
}
