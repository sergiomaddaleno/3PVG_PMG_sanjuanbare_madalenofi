
#include <memory> 
#include <iostream>
#include <cassert>
#include "vector"

#include "utils.h"
#include "../deps/dds.h"

char* ReadFile(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    long size = 0;
    if (file) {
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* my_file = (char*)malloc(size + 1);
        if (my_file) {
            fread(my_file, sizeof(char), size, file);
            my_file[size] = '\0';

            //printf("%s\n", my_file);
        }
        fclose(file);

        return my_file;
    }
    else {
        printf("Error in loading file\n");
    }

    return nullptr;
}

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT (0x83F0)

unsigned int UploadTexture(char* dds_file)
{
    assert(dds_file[0] == 'D' && dds_file[1] == 'D' && dds_file[2] == 'S' && dds_file[3] == ' ');
    DDSHeader* head = (DDSHeader*)&dds_file[4];
    unsigned char* pixels = (unsigned char*)&head[1];

    //assert(GL_EXT_texture_compression_s3tc);

    unsigned int tex = 0;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

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
    return tex;
}


static char infoLog[512];
static void CheckGPUErrors(unsigned int code, const char* str, bool link)
{
    int success = 0;
    if (link) {
        glGetProgramiv(code, GL_LINK_STATUS, &success);
        if (!success)
            glGetProgramInfoLog(code, 512, NULL, infoLog);
    }
    else {
        glGetShaderiv(code, GL_COMPILE_STATUS, &success);
        if (!success)
            glGetShaderInfoLog(code, 512, NULL, infoLog);
    }
    if (!success) {
        std::cout << str << infoLog << std::endl;
    }
    assert(success);
}
