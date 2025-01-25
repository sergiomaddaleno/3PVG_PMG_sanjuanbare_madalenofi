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

public:
    Texture() : textureID(0), width(0), height(0), channels(0) {}
    ~Texture() {}

    bool loadTexture(const std::string& file_path);

    bool loadSkyboxTex(char* dds_file);

    void bind() const;

    //--

    GLuint getID() const {return textureID;}

    Texture getTexture() const {
        Texture tex;
        tex.textureID = this->textureID;
        return tex;
    }

    //Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        json_data["textureID"] = textureID;

        return json_data;
    }

    nlohmann::json deSerialize(const nlohmann::json& j) const {
        j.at("textureID").get<GLuint>();
        return j;
    }
};

#endif
