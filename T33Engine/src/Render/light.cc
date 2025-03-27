
#include "Render/light.h"
#include "Core/shader.h"

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

void Light::setUniforms(Shader& shader)
{
    int loc = shader.getUniformLocation("u_light.type_");
    shader.setUniformValue(loc, ValueType::T_INT_1, &type_);

    loc = shader.getUniformLocation("u_light.pos_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_3, &pos_);

    loc = shader.getUniformLocation("u_light_pos_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_3, &pos_);

    loc = shader.getUniformLocation("u_light.dir_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_3, &dir_);

    loc = shader.getUniformLocation("u_light.light_color_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_3, &light_color_);

    loc = shader.getUniformLocation("u_light.ambient_color_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_3, &ambient_color_);

    loc = shader.getUniformLocation("u_light.linear_att_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &linear_att);

    loc = shader.getUniformLocation("u_light.quad_att_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &quad_att_);

    loc = shader.getUniformLocation("u_light.constant_att_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &constant_att_);

    loc = shader.getUniformLocation("u_light.shininess_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &shininess_);

    loc = shader.getUniformLocation("u_light.strength_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &strength_);

    loc = shader.getUniformLocation("u_light.spot_angle_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &spot_angle_);

    loc = shader.getUniformLocation("u_light.spot_softness_");
    shader.setUniformValue(loc, ValueType::T_FLOAT_1, &spot_softness_);

    //shader.setUniformValue(shader.getUniformLocation("u_ambient"), ValueType::T_FLOAT_3, &global_ambient_color_);
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
