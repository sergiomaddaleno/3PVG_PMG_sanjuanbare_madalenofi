
#include "Render/skybox.h"
#include "Core/mesh.h"
#include "Core/Camera.h"

#include "Components/render_component.h"
#include "utils.h"

#include <../deps/glm/glm.hpp>
#include <../deps/glm/gtc/matrix_transform.hpp>
#include <../deps/glm/gtc/type_ptr.hpp>

#include "../deps/dds.h"
#include "../deps/stb_image.h"
#include <cassert>

#include <memory> 
#include <iostream>
#include <vector>
#include <string>

static const glm::vec3 dds_vertices[] = {
  {-1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f, -1.0f},
  {1.0f,  1.0f, -1.0f},
  {-1.0f,  1.0f, -1.0f},

  {-1.0f, -1.0f,  1.0f},
  {1.0f, -1.0f,  1.0f},
  {1.0f,  1.0f,  1.0f},
  {-1.0f,  1.0f,  1.0f},

  {-1.0f,  1.0f,  1.0f},
  {-1.0f,  1.0f, -1.0f},
  {-1.0f, -1.0f, -1.0f},
  {-1.0f, -1.0f,  1.0f},

  {1.0f,  1.0f,  1.0f},
  {1.0f,  1.0f, -1.0f},
  {1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f,  1.0f},

  {-1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f, -1.0f},
  {1.0f, -1.0f,  1.0f},
  {-1.0f, -1.0f,  1.0f},

  {-1.0f,  1.0f, -1.0f},
  {1.0f,  1.0f, -1.0f},
  {1.0f,  1.0f,  1.0f},
  {-1.0f,  1.0f,  1.0f},
};

#define _CF(A)   A,A+1,A+2,A+2,A+3,A+0
#define _SF(A)   A,A+2,A+1,A+2,A+0,A+3  

static const unsigned int dds_indices[] = {
  _SF(0),
  _CF(4),
  _CF(8),
  _SF(12),
  _CF(16),
  _SF(20)
};

float cubemap_vertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

Skybox::Skybox() : dds_angle_(-90.0f), cubemapVAO(0), cubemapVBO(0), skyboxVAO(0), VBO(0), EBO(0),
dds_file_(), 
dds_vertex_file_(), 
dds_fragment_file_(),
use_cubemap(true)
{}

Skybox::~Skybox()
{

}

void Skybox::initDDS() {

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dds_vertices), dds_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(dds_indices), dds_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::initcubeMap()
{
    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemap_vertices), &cubemap_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Skybox::render(Camera cam)
{
    if (use_cubemap == true) {
        renderCubeMap(cam);
    }
    else {
        renderDDS(cam);
    }
}

void Skybox::renderDDS(Camera cam) {
    glDepthMask(GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_LEQUAL);

    glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
    glm::mat4 projection = cam.getPerspectiveMatrix();
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(dds_angle_), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 model = rotation;

    int tex_id = 1;
    dds_shader.useProgram();

    dds_texture_.active();
    dds_texture_.bind2D();
    dds_shader.setUniformValue(dds_shader.getUniformLocation("skybox"), ValueType::T_INT_1, &tex_id);
    dds_shader.setUniformValue(dds_shader.getUniformLocation("view"), ValueType::T_MAT_4x4, &view[0][0]);
    dds_shader.setUniformValue(dds_shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection[0][0]);
    dds_shader.setUniformValue(dds_shader.getUniformLocation("model"), ValueType::T_MAT_4x4, &model[0][0]);

    glBindVertexArray(skyboxVAO);
    glDrawElements(GL_TRIANGLES, sizeof(dds_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);
}

void Skybox::renderCubeMap(Camera cam)
{
    glDepthFunc(GL_LEQUAL);

    glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
    glm::mat4 projection = cam.getPerspectiveMatrix();

    int tex_id = 1;
    cubemap_shader.useProgram();

    cubemap_texture_.active();
    cubemap_texture_.bindCubeMap();
    cubemap_shader.setUniformValue(cubemap_shader.getUniformLocation("skybox"), ValueType::T_INT_1, &tex_id);
    cubemap_shader.setUniformValue(cubemap_shader.getUniformLocation("view"), ValueType::T_MAT_4x4, &view[0][0]);
    cubemap_shader.setUniformValue(cubemap_shader.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection[0][0]);

    glBindVertexArray(cubemapVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::loadDDS(const char* dds_file, const char* vertex_file, const char* fragment_file)
{
    dds_file_ = dds_file;
    dds_vertex_file_ = vertex_file;
    dds_fragment_file_ = fragment_file;

    char* loc_file = ReadFile(dds_file);
    char* loc_vert = ReadFile(vertex_file);
    char* loc_frag = ReadFile(fragment_file);

    if (!loadDDSFile(loc_file)) {
        printf("Error in loading skybox\n");
    }

    dds_shader.compile(loc_vert, loc_frag);

    free(loc_file);
    free(loc_vert);
    free(loc_frag);
}

void Skybox::loadCubeMap(std::vector<std::string> faces_files, const char* vertex_file, const char* fragment_file)
{
    faces_files_ = faces_files;
    cubemap_vertex_file_ = vertex_file;
    cubemap_fragment_file_ = fragment_file;

    char* loc_vert = ReadFile(vertex_file);
    char* loc_frag = ReadFile(fragment_file);

    cubemap_shader.compile(loc_vert, loc_frag);

    // ---

    cubemap_texture_.generate();
    cubemap_texture_.bindCubeMap();

    int width, height, channels;
    for (unsigned int i = 0; i < faces_files.size(); ++i) {
        unsigned char* data = stbi_load(faces_files[i].c_str(), &width, &height, &channels, 0);
        if (data) {

            std::string extension = faces_files[0].substr(faces_files[0].find_last_of("."));

            if (extension == ".png") {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }

            if (extension == ".jpg") {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
        }
        else {
            printf("Cubemap error in loading.\n");
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    free(loc_vert);
    free(loc_frag);
}

bool Skybox::loadDDSFile(const char* dds_file)
{
    if (dds_file == nullptr) {
        return false;
    }

    if (!(dds_file[0] == 'D' && dds_file[1] == 'D' && dds_file[2] == 'S' && dds_file[3] == ' ')) {
        return false;
    }
    DDSHeader* head = (DDSHeader*)&dds_file[4];
    unsigned char* pixels = (unsigned char*)&head[1];

    //assert(GL_EXT_texture_compression_s3tc);

    if (dds_texture_.getID() != 0) {
        dds_texture_.deleteTexture();
        dds_texture_.setID(0);
    }

    dds_texture_.generate();
    dds_texture_.bind2D();

    // Esta es la configuracion tipica de "sampling" habitual, para cuando un poligono se ve lejos y cerca
#ifdef FILTROS
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
  // Esta seria la minima posible
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif
    // Configuramos lo que debe ocurrir cuando una textura se sale de los limites durante el pintado
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int height = head->height;
    int width = head->width;
    assert(width == height);
    int size = (width * height) / 2; // DXT1 pixels 4bpp
    unsigned int i;
    for (i = 0; i < head->mipMapCount; ++i) {
        glCompressedTexImage2D(GL_TEXTURE_2D, i, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, width, height, 0, size, pixels);
        //assert(glGetError() == GL_NO_ERROR);

        pixels += size;
        if (width > 4)  // En DXT las texturas no pueden ser menores a 8
            size >>= 2;    // Cada mipmap es 1/4 del anterior

        width >>= 1,    // Partimos las dimensiones a la mitad, para el siguiente mip map
            height >>= 1;
    }

    return true;
}

nlohmann::json Skybox::serialize() const
{
    nlohmann::json json_data;

    json_data["Use_cubemap"] = use_cubemap;

    json_data["DDS_fragment_file"] = dds_fragment_file_;
    json_data["DDS_vertex_file"] = dds_vertex_file_;
    json_data["DDS_file_path"] = dds_file_;
    json_data["DDS_angle"] = dds_angle_;

    json_data["Cubemap_vertex_file"] = cubemap_vertex_file_;
    json_data["Cubemap_fragment_file"] = cubemap_fragment_file_;
    json_data["Cubemap_file_paths"] = { 
        faces_files_[0], 
        faces_files_[1], 
        faces_files_[2], 
        faces_files_[3], 
        faces_files_[4], 
        faces_files_[5],
    };

    return json_data;
}

