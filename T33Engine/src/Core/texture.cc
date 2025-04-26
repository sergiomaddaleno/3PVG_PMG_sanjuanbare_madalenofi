
#include "Core/texture.h"
#include "utils.h"
#include "../deps/stb_image.h"
#include <cassert>

Texture::Texture() : textureID(0), width(0), height(0), channels(0), data_(nullptr) {}

Texture::~Texture()
{
    /*if (data_ != nullptr) {
        stbi_image_free(data_);
    }*/
}

bool Texture::loadTexture(const std::string& file_path) {
    if (file_path.empty()) return false;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
    data_ = data;
    if (!data) {
        AddConsoleMessage("Failed to load texture " + file_path);
        return false;
    }

    this->generate();
    this->bind2D();

    GLenum format = 1;
    switch (channels) {
    case 1: format = GL_RED;  break;
    case 2: format = GL_RG;   break;
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

bool Texture::loadTexture3D(const std::string& file_path, int slices) {
    if (file_path.empty()) return false;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        AddConsoleMessage("Failed to load texture " + file_path);
        return false;
    }

    if (slices <= 0) slices = width / height; 

    if (width * height * channels % slices != 0) {
        printf("Error");
        AddConsoleMessage("Error");
        stbi_image_free(data);
        return false;
    }

    this->generate();
    this->bind3D();

    GLenum format = 1;
    switch (channels) {
    case 1: format = GL_RED;  break;
    case 2: format = GL_RG;   break;
    case 3: format = GL_RGB;  break;
    case 4: format = GL_RGBA; break;
    }

    int slice_size = width * height * channels / slices;
    glTexImage3D(GL_TEXTURE_3D, 0, format, width, height / slices, slices, 0, format, GL_UNSIGNED_BYTE, data );

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}

void Texture::createColorTexture(const glm::vec3& color) {

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    unsigned char data[3] = {
        (unsigned char)(color.r * 255),
        (unsigned char)(color.g * 255),
        (unsigned char)(color.b * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::generateNoise3D(int width, int height, int depth) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_3D, textureID);

    std::vector<GLubyte> data(width * height * depth);

    for (int z = 0; z < depth; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                float fx = x / (float)width;
                float fy = y / (float)height;
                float fz = z / (float)depth;

                float noise = 0.0f;
                noise += sinf(fx * 5 + fz * 3) * 0.5f + 0.5f;
                noise += cosf(fy * 7 + fx * 2) * 0.25f + 0.25f;
                noise += sinf(fz * 9 + fy * 4) * 0.125f + 0.125f;

                data[x + y * width + z * width * height] =
                    static_cast<GLubyte>(noise * 255);
            }
        }
    }

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width, height, depth, 0,
        GL_RED, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_3D);

    return true;
}


void Texture::generate()
{
    glGenTextures(1, &textureID);
}

void Texture::active(unsigned int num) {
    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0 + num);
    }
}

void Texture::bind2D() const
{
    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
}

void Texture::bind3D() const
{
    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_3D, textureID);
    }
}

void Texture::bindCubeMap() const
{
    if (textureID != 0) {
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    }
}

void Texture::deleteTexture()
{
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}

