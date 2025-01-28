
#include "Core/texture.h"

#include "../deps/stb_image.h"
#include "../deps/dds.h"
#include <cassert>

bool Texture::loadTexture(const std::string& file_path) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
    data_ = data;
    if (!data) {
        printf("Failed to load texture: %s\n", file_path.c_str());
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, textureID);
    stbi_image_free(data);

    return true;
}

bool Texture::loadSkyboxTex(char* dds_file)
{
    assert(dds_file[0] == 'D' && dds_file[1] == 'D' && dds_file[2] == 'S' && dds_file[3] == ' ');
    DDSHeader* head = (DDSHeader*)&dds_file[4];
    unsigned char* pixels = (unsigned char*)&head[1];

    //assert(GL_EXT_texture_compression_s3tc);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

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
        assert(glGetError() == GL_NO_ERROR);

        pixels += size;
        if (width > 4)  // En DXT las texturas no pueden ser menores a 8
            size >>= 2;    // Cada mipmap es 1/4 del anterior

        width >>= 1,    // Partimos las dimensiones a la mitad, para el siguiente mip map
            height >>= 1;
    }

    return true;
}

void Texture::bind() const
{
    if (textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
}