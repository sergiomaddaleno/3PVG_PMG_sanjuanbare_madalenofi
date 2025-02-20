
#include "Render/skybox.h"
#include "Core/mesh.h"
#include "Render/Camera.h"

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

Skybox::Skybox() : skybox_angle_(-90.0f), cubeVAO(0), skyboxVAO(0), VBO(0), EBO(0),
dds_file_(), 
vertex_file_(), 
fragment_file_()
{}

Skybox::~Skybox()
{
    if (skyboxVAO) glDeleteVertexArrays(1, &skyboxVAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Skybox::init() {

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gMesh), gMesh, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gIndices), gIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::renderDDS(Camera cam) {
    glDepthMask(GL_FALSE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glEnable(GL_LEQUAL);

    glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
    glm::mat4 projection = cam.getPerspectiveMatrix();
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(skybox_angle_), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 model = rotation;

    int tex_id = 1;
    shader_.useProgram();

    texture_.bind2D();
    shader_.setUniformValue(shader_.getUniformLocation("skybox"), ValueType::T_INT_1, &tex_id);
    shader_.setUniformValue(shader_.getUniformLocation("view"), ValueType::T_MAT_4x4, &view[0][0]);
    shader_.setUniformValue(shader_.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection[0][0]);
    shader_.setUniformValue(shader_.getUniformLocation("model"), ValueType::T_MAT_4x4, &model[0][0]);

    glBindVertexArray(skyboxVAO);
    glDrawElements(GL_TRIANGLES, sizeof(gIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);
}

void Skybox::renderCubeMap(Camera cam)
{
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glm::mat4 view = glm::mat4(glm::mat3(cam.getViewMatrix()));
    glm::mat4 projection = cam.getPerspectiveMatrix();

    int tex_id = 1;
    shader_.useProgram();

    texture_.bindCubeMap();
    shader_.setUniformValue(shader_.getUniformLocation("skybox"), ValueType::T_INT_1, &tex_id);
    shader_.setUniformValue(shader_.getUniformLocation("view"), ValueType::T_MAT_4x4, &view[0][0]);
    shader_.setUniformValue(shader_.getUniformLocation("projection"), ValueType::T_MAT_4x4, &projection[0][0]);

    glBindVertexArray(skyboxVAO);
    glDrawElements(GL_TRIANGLES, sizeof(gIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    glUseProgram(0);
}

void Skybox::loadDDS(const char* dds_file, const char* vertex_file, const char* fragment_file)
{
    dds_file_ = dds_file;
    vertex_file_ = vertex_file;
    fragment_file_ = fragment_file;

    char* loc_file = ReadFile(dds_file);
    char* loc_vert = ReadFile(vertex_file);
    char* loc_frag = ReadFile(fragment_file);

    if (!loadDDSFile(loc_file)) {
        printf("Error in loading skybox\n");
    }

    shader_.compile(loc_vert, loc_frag);

    free(loc_file);
    free(loc_vert);
    free(loc_frag);
}

void Skybox::loadCubeMap(std::vector<std::string> faces_files, const char* vertex_file, const char* fragment_file)
{
    faces_files_ = faces_files;
    vertex_file_ = vertex_file;
    fragment_file_ = fragment_file;

    char* loc_vert = ReadFile(vertex_file);
    char* loc_frag = ReadFile(fragment_file);

    shader_.compile(loc_vert, loc_frag);

    // ---

    texture_.generate();
    texture_.bindCubeMap();

    int width, height, channels;
    for (unsigned int i = 0; i < faces_files.size(); ++i) {
        unsigned char* data = stbi_load(faces_files[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load cubemap texture at path: " << faces_files[i] << std::endl;
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

    if (texture_.getID() != 0) {
        texture_.deleteTexture();
        texture_.setID(0);
    }

    texture_.generate();
    texture_.bind2D();

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

    json_data["Skybox_file_path"] = dds_file_;
    json_data["Vertex_source"] = vertex_file_;
    json_data["Fragment_source"] = fragment_file_;
    json_data["angle"] = skybox_angle_;

    return json_data;
}

