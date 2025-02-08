
#ifndef LIT
#define LIT

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../nlohmann/json.hpp>

struct LightComponent;
class Shader;

enum LightType {
    LT_Directional,
    LT_Spot,
    LT_Point,
};

struct LightInfo {

    LightType type_;
    
    // --
    glm::vec3 pos_;
    glm::vec3 dir_;
    glm::vec3 diff_color_;
    glm::vec3 spec_color_;
    glm::vec3 light_color_;
    glm::vec3 ambient_color_;

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle_;

    LightInfo() : 
        type_(LightType::LT_Spot),
        pos_(0.0f, 0.0f, 0.0f),
        dir_(0.0f, 0.0f, 0.0f),
        diff_color_(0.0f, 0.0f, 0.0f),
        spec_color_(0.0f, 0.0f, 0.0f),
        light_color_(0.0f, 0.0f, 0.0f),
        ambient_color_(0.0f, 0.0f, 0.0f),
        linear_att(0.0f),
        quad_att_(0.0f),
        constant_att_(0.0f),
        shininess_(0.0f),
        strength_(0.0f),
        spot_angle_(0.0f)
    {}

    ~LightInfo(){}
};

class Light {

public:

    Light();
    ~Light();

    LightInfo light_info_;
    glm::vec3 global_ambient_color_{ 0.2f, 0.2, 0.2f };
    int light_count = 0;

    void setUniforms(Shader& shader, int light_count);

    // Save data
    nlohmann::json serialize() const;

};

#endif // LIT