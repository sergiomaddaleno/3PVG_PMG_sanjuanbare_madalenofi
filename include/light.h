
#ifndef LIT
#define LIT

#include <optional>
#include <string>
#include <../deps/glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "..\include\Components\transform_component.h"
#include "..\include\Components\render_component.h"


class Light {

public:

    Light();

    //std::vector<Light> lights;
    int type_; //0=dir, 1=spot, 2=point
    int enabled_; //false by default
    glm::vec3 pos_;
    glm::vec3 dir_;

    glm::vec3 diff_color_;
    glm::vec3 spec_color_;
    glm::vec3 light_color_;
    glm::vec3 ambient_color;

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle;
    float camera_pos_[3];
    float light_linear_;
    float light_quadratic_;
};

#endif // LIT