
#include "Render/light.h"

#include "Components\light_component.h"
#include "utils.h"

#include <memory> 
#include <iostream>

Light::Light() : 
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
    spot_angle_(0.0f),
    spot_softness_(0.0f)
{
}

Light::~Light()
{
}

nlohmann::json Light::serialize() const
{
    nlohmann::json json_data;

    json_data["Type"] = (int) type_;

    json_data["Position"].push_back({  pos_.x,  pos_.y,  pos_.z });
    json_data["Direction"].push_back({  dir_.x,  dir_.y,  dir_.z });
    json_data["Diffusion_color"].push_back({  diff_color_.x,  diff_color_.y,  diff_color_.z });
    json_data["Specular_color"].push_back({  spec_color_.x,  spec_color_.y,  spec_color_.z });
    json_data["Light_color"].push_back({  light_color_.x,  light_color_.y,  light_color_.z });
    json_data["Ambient_color"].push_back({  ambient_color_.x,  ambient_color_.y,  ambient_color_.z });

    json_data["Linear_attenuation"].push_back( linear_att);
    json_data["Quadratic_attenuation"].push_back( quad_att_);
    json_data["Constant_attenuation"].push_back( constant_att_);
    json_data["Shininnes"].push_back( shininess_);
    json_data["Strength"].push_back( strength_);
    json_data["Spot_angle"].push_back( spot_angle_);
    json_data["Spot_softness"].push_back(spot_softness_);

    return json_data;
}
