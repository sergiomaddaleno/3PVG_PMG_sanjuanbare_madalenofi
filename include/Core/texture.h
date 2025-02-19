#ifndef __TEXTURE__
#define __TEXTURE__

#include <string>
#include <vector>
#include "../deps/nlohmann/json.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Texture {

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

private:
    GLuint textureID;

    int width, height, channels;
    unsigned char* data_;
};

#endif // __TEXTURE__
