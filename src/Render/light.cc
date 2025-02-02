
#include "Render/light.h"
#include "Core/shader.h"

#include "Components\light_component.h"
#include "utils.h"

#include <memory> 
#include <iostream>

void Light::setUniforms(Shader &shader, int light_count)
{
    int loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].type_").c_str());
    shader.setUniformValue(loc, Type::T_INT_1, &light_info_.type_);

    loc =  shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].pos_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_3, & light_info_.pos_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].dir_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_3, &light_info_.dir_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].light_color_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_3, &light_info_.light_color_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].ambient_color_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_3, &light_info_.ambient_color_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].linear_att_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_1, &light_info_.linear_att);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].quad_att_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_1, &light_info_.quad_att_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].constant_att_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_1, &light_info_.constant_att_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].shininess_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_1, &light_info_.shininess_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].strength_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_1, &light_info_.strength_);

     loc = shader.getUniformLocation(("u_light[" + std::to_string(light_count) + "].spot_angle_").c_str());
     shader.setUniformValue(loc, Type::T_FLOAT_1, & light_info_.spot_angle_);
}
