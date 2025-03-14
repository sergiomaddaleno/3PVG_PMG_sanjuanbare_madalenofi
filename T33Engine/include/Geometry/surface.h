/**
 * @file surface.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __SURFACE__
#define __SURFACE__ 1

#include <memory>
#include <vector>

#include "Core/texture.h"
#include "Geometry/wave.h"
#include <../nlohmann/json.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Surface
  * @brief Represents a custom surface.
  */

class Surface {
public:
    Surface();
    ~Surface();

    void init(std::vector<float> surface_points,
        const int num_heights = 4, const int num_revs = 4);

    void bind();
    void draw();
    void setTexture(std::string file_path);
    std::vector<float> getVertex();

    nlohmann::json serialize() const;

    // --

    int num_heights_, num_revs_;
    std::vector<float> surface_points_;
    Texture texture_;
    std::string texture_path_;
    bool use_texture_;

    GerstnerWave wave_;

private:
    GLuint VAO, VBO, EBO;

    int num_vertex_;
    std::vector<float> vertex_;
    std::vector<float> normals_;
    std::vector<float> uv_;

    std::vector<int> order_;
    std::vector<float> data_;
};

#endif // !__SURFACE__