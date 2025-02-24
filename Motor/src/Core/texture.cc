
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

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
    data_ = data;
    if (!data) {
        printf("Failed to load texture: %s\n", file_path.c_str());
        return false;
    }

    generate();

    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textureID);

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

