
#ifndef __LIGHT__
#define __LIGHT__

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../nlohmann/json.hpp>

class Shader;

enum LightType {
    LT_Directional,
    LT_Spot,
    LT_Point,
};

class Light {

public:

    Light();
    ~Light();

    void setUniforms(Shader& shader);

    LightType type_;

    glm::vec3 pos_;
    glm::vec3 dir_;
    glm::vec3 diff_color_;
    glm::vec3 spec_color_;
    glm::vec3 light_color_;
    glm::vec3 ambient_color_;
    glm::vec3 global_ambient_color_{ 0.2f, 0.2, 0.2f };

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle_;
    float spot_softness_;

    nlohmann::json serialize() const;
};

#endif // __LIGHT__