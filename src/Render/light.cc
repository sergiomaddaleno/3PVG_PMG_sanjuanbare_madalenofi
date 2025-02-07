
#include "Render/light.h"
#include "Core/shader.h"

#include "Components\light_component.h"
#include "utils.h"

#include <memory> 
#include <iostream>

void Light::setUniforms(Shader &shader, int light_count,LightComponent &light_comp)
{

    if (light_count > 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_comp.depthMap);
    }
     std::string uniformName = "u_shadow_map[" + std::to_string(light_count+1) + "]";
     int locshadow = shader.getUniformLocation(uniformName.c_str());
     shader.setUniformValue(locshadow, ValueType::T_FLOAT_1, &light_comp.depthMap);

    int loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].type_").c_str());
    shader.setUniformValue(loc, ValueType::T_INT_1, &light_info_.type_);

    loc =  shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].pos_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_3, & light_info_.pos_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].dir_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_3, &light_info_.dir_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].light_color_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_3, &light_info_.light_color_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].ambient_color_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_3, &light_info_.ambient_color_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].linear_att_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_1, &light_info_.linear_att);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].quad_att_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_1, &light_info_.quad_att_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].constant_att_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_1, &light_info_.constant_att_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].shininess_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_1, &light_info_.shininess_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].strength_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_1, &light_info_.strength_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].spot_angle_").c_str());
     shader.setUniformValue(loc, ValueType::T_FLOAT_1, & light_info_.spot_angle_);
}

nlohmann::json Light::serialize() const
{
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
