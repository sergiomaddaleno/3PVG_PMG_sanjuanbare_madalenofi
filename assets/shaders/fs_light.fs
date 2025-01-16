#version 330

out vec4 fragColor;

in vec3 normal;
in vec3 pos;
in vec2 uv;
in vec3 FragPos;

struct LightInfo {
    int type_; // 0 = Spot, 1 = Point
    vec3 pos_;
    vec3 dir_;
    vec3 diff_color_;
    vec3 spec_color_;
    vec3 light_color_;
    vec3 ambient_color;

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle;
   
};

const int kMaxLights = 8;
uniform LightInfo u_light[kMaxLights];
uniform int u_number_lights;
uniform vec3 u_camera_position;
uniform vec3 u_ambient;


uniform sampler2D texture1;

vec3 PointLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float diffuseStrength = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lc.diff_color_ * diffuseStrength * attenuation;

    vec3 viewDir = normalize(u_camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);

    vec3 specular = specStrength * lc.spec_color_ * attenuation;
    return diffuse + specular;
}

vec3 SpotLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float cutOff = cos(radians(lc.spot_angle));
    float theta = dot(lightDir, normalize(-lc.dir_));

    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    if (theta > cutOff) {
        float diffuseStrength = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lc.diff_color_ * diffuseStrength * attenuation;

        vec3 viewDir = normalize(u_camera_position - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);
        float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);
        vec3 specular = specStrength * lc.spec_color_ * attenuation;

        return diffuse + specular;
    } else {
        return lc.ambient_color * u_ambient;
    }
}

void main() {
    vec3 final_color = u_ambient;

    

   // for (int i = 0; i < 2; ++i) {
        if (u_light[0].type_ == 0) {
            final_color += SpotLight(u_light[0]);
        } else if (u_light[0].type_ == 1) {
            final_color += PointLight(u_light[0]);
        }
    //}

     vec4 textureColor = texture(texture1, uv);

    
    vec3 combinedColor = textureColor.rgb * final_color;
    
    fragColor = vec4(combinedColor, 1.0);
}
