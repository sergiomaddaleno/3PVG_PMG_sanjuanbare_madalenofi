#ifndef __TEXTURE__
#define __TEXTURE__

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../deps/nlohmann/json.hpp"

class Texture {
private:
    GLuint textureID;

    int width, height, channels;

    unsigned char* data_;

public:
    Texture();
    ~Texture();

    bool loadTexture(const std::string& file_path);

    bool loadSkyboxTexture(const char* dds_file);

    void bind() const;

    //--

    GLuint getID() const {return textureID;}
    int getwidth() const { return width; }
    int getHeight() const { return height; }
    unsigned char* getData() const { return data_; }
    int getChannels() const { return channels; }

    //Save data
    nlohmann::json serialize() const;
};

#endif // __TEXTURE__
