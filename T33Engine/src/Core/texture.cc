
#include "Core/texture.h"

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
        printf("Failed to load texture: %s\n", file_path.c_str());
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
        printf("Failed to load texture: %s\n", file_path.c_str());
        return false;
    }

    if (slices <= 0) slices = width / height; 

    if (width * height * channels % slices != 0) {
        printf("Error");
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

