/**
 * @file texture.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __TEXTURE__
#define __TEXTURE__

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

 /**
  * @class Texture
  * @brief Encapsulates texture loading, generation, and binding for OpenGL.
  */
class Texture {

public:
    Texture();
    ~Texture();

    void generate();
    void active(unsigned int num = 1);
    void bind2D() const;
    void bind3D() const;
    void bindCubeMap() const;
    void deleteTexture();

    bool loadTexture(const std::string& file_path);
    bool loadTexture3D(const std::string& file_path, int slices = 0);

    bool generateNoise3D(int width = 64, int height = 64, int depth = 64);

    //--
    void setID(GLuint texID) { textureID = texID; }

    GLuint getID() const {return textureID;}
    int getwidth() const { return width; }
    int getHeight() const { return height; }
    unsigned char* getData() const { return data_; }
    int getChannels() const { return channels; }

private:
    GLuint textureID;

    int width, height, channels;
    unsigned char* data_;
};

#endif // !__TEXTURE__
