
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

    void setUniforms(Shader& shader, int light_count);

    LightInfo light_info_;
    glm::vec3 global_ambient_color_{ 0.2f, 0.2, 0.2f };

    // Save data
    nlohmann::json serialize() const {
        nlohmann::json json_data;

        json_data["Type"] = (int)light_info_.type_;

        json_data["Position"].push_back({ light_info_.pos_.x, light_info_.pos_.y, light_info_.pos_.z });
        json_data["Direction"].push_back({ light_info_.dir_.x, light_info_.dir_.y, light_info_.dir_.z });
        json_data["Diffusion_color"].push_back({ light_info_.diff_color_.x, light_info_.diff_color_.y, light_info_.diff_color_.z });
        json_data["Specular_color"].push_back({ light_info_.spec_color_.x, light_info_.spec_color_.y, light_info_.spec_color_.z });
        json_data["Light_color"].push_back({ light_info_.light_color_.x, light_info_.light_color_.y, light_info_.light_color_.z });
        json_data["Ambient_color"].push_back({ light_info_.ambient_color_.x, light_info_.ambient_color_.y, light_info_.ambient_color_.z });

        json_data["Linear_attenuation"].push_back(light_info_.linear_att);
        json_data["Quadratic_attenuation"].push_back(light_info_.quad_att_);
        json_data["Constant_attenuation"].push_back(light_info_.constant_att_);
        json_data["Shininnes"].push_back(light_info_.shininess_);
        json_data["Strength"].push_back(light_info_.strength_);
        json_data["Spot_angle"].push_back(light_info_.spot_angle_);

        return json_data;
    }

};

#endif // LIT