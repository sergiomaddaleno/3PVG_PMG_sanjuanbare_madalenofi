/**
 * @file light.h
 * @authors madalenofi@esat-alumni.com
 * @authors sanjuanbare@esat-alumni.com
 * @date 2025-03-05
 * @version 1.0
 */

#ifndef __LIGHT__
#define __LIGHT__

#include <optional>
#include <string>
#include "Core/shader.h"

#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <../nlohmann/json.hpp>

class Shader;

/**
 * @class Light
 * @brief Represents a light source in a 3D scene.
 */
class Light {

public:

    enum LightType {
        LT_Directional,
        LT_Spot,
        LT_Point,
    };

    Light();
    ~Light();

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

    void setUniforms(Shader& shader) {
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

    nlohmann::json serialize() const;
};

#endif // !__LIGHT__