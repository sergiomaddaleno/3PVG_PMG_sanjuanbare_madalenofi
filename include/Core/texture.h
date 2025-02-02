#ifndef TEX
#define TEX

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
    Texture() : textureID(0), width(0), height(0), channels(0), data_(nullptr) {}
    ~Texture() {}

    bool loadTexture(const std::string& file_path);

    bool loadSkyboxTex(char* dds_file);

    void bind() const;

    //--

    GLuint getID() const {return textureID;}
    int getwidth() const { return width; }
    int getHeight() const { return height; }
    unsigned char* getData() const { return data_; }

    Texture getTexture() const {
        Texture tex;
        tex.textureID = this->textureID;
        return tex;
    }

    //Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        json_data["TextureID"] = textureID;

        return json_data;
    }
};

#endif
