
#ifndef LIT
#define LIT

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../nlohmann/json.hpp>

enum LightType {
    LT_Directional,
    LT_Spot,
    LT_Point,
};

struct LightInfo {

    LightType type_;

    // --

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

    // Save data
    nlohmann::json serialize() const{
        nlohmann::json json_data;

        json_data["Type"] = (int)type_;

        json_data["Direction"].push_back({ dir_.x, dir_.y, dir_.z });
        json_data["Diffusion Color"].push_back({ diff_color_.x, diff_color_.y, diff_color_.z });
        json_data["Specular Color"].push_back({ spec_color_.x, spec_color_.y, spec_color_.z });
        json_data["Light Color"].push_back({ light_color_.x, light_color_.y, light_color_.z });
        json_data["Ambient Color"].push_back({ ambient_color_.x, ambient_color_.y, ambient_color_.z });

        json_data["Linear attenuation"].push_back(linear_att);
        json_data["Quadratic attenuation"].push_back(quad_att_);
        json_data["Constant attenuation"].push_back(constant_att_);
        json_data["Shininnes"].push_back(shininess_);
        json_data["Strength"].push_back(strength_);
        json_data["Spot Angle"].push_back(spot_angle_);

        return json_data;
    }

    nlohmann::json deSerialize(nlohmann::json& j) {
       
        type_ = j["Type"].get<LightType>();

        //-----------
        std::vector<float> temp_vec3;

        temp_vec3 = j["Direction"].get<std::vector<float>>();
        dir_ = { temp_vec3[0], temp_vec3[1], temp_vec3[2] };

        temp_vec3 = j["Diffusion Color"].get<std::vector<float>>();
        diff_color_ = { temp_vec3[0], temp_vec3[1], temp_vec3[2] };

        temp_vec3 = j["Specular Color"].get<std::vector<float>>();
        spec_color_ = { temp_vec3[0], temp_vec3[1], temp_vec3[2] };

        temp_vec3 = j["Light Color"].get<std::vector<float>>();
        light_color_ = { temp_vec3[0], temp_vec3[1], temp_vec3[2] };

        temp_vec3 = j["Ambient Color"].get<std::vector<float>>();
        ambient_color_ = { temp_vec3[0], temp_vec3[1], temp_vec3[2] };
        //-----------

        linear_att = j["Linear attenuation"].get<float>();
        quad_att_ = j["Quadratic attenuation"].get<float>();
        constant_att_ = j["Constant attenuation"].get<float>();
        shininess_ = j["Shininnes"].get<float>();
        strength_ = j["Strength"].get<float>();
        spot_angle_ = j["Spot Angle"].get<float>();

        return j;
    }

    // --

    LightInfo() : 
        type_(LightType::LT_Spot),
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

static glm::vec3 global_ambient_color_{ 0.2f, 0.2, 0.2f };


#endif // LIT