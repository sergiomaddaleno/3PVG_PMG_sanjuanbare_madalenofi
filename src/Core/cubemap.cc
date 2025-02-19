
#include "Core/cubemap.h"
#include "../deps/stb_image.h"

Cubemap::Cubemap()
{
}

Cubemap::~Cubemap()
{
}

void Cubemap::init()
{
	/*glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

    for (unsigned int i = 0; i < textures_faces.size(); i++)
    {
        data_ = stbi_load(textures_faces[i].c_str(), &width, &height, &channels, 0);
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                      0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data_
        );
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;*/
}

void Cubemap::bind()
{
}
