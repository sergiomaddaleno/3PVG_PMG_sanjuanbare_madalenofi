#version 330

out vec4 fragColor;

in vec3 normal;
in vec3 pos;
in vec2 uv;

struct LightInfo {
    int type_; //0=dir, 1=spot, 2=point
    vec3 pos_;
    vec3 dir_;
    vec3 diff_color_;
    vec3 spec_color_;
    vec3 light_color_;
    
    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle;
    vec3 ambient_color;

    float light_linear_;
    float light_quadratic_;
    //uniform float camera_pos_[3];
    //uniform int enabled_; //false by default
};

const int kMaxLights = 8;
uniform LightInfo u_light[kMaxLights]; //Max allowed lights.


uniform int u_number_lights;       //Number of lights to process.
uniform vec3 u_camera_position;
uniform vec3 u_ambient;

vec3 PointLight(LightInfo lc) {
    lc.dir_ = normalize(lc.pos_ - pos);

    // Calculate attenuation
    float constant = 1.0;
    float linear = 0.027;
    float quadratic = 0.07;

    float distance = length(lc.pos_ - pos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // Diffuse reflection
    float diffuseStrength = max(dot(lc.dir_, normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation;

    // Specular reflection
    vec3 viewDir = normalize(u_camera_position - pos);
    vec3 reflectDir = reflect(-lc.dir_, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 specular = specStrength * lc.spec_color_ * attenuation;

    // Combine diffuse and specular components
    vec3 result = (diffuse + specular);

    return result;
}

vec3 SpotLight(LightInfo lc) {
    vec3 specular_color = lc.spec_color_;
    vec3 light_color = lc.light_color_;
    vec3 light_pos = lc.pos_;
    vec3 light_dir = normalize(light_pos - pos);
    float cutOff = cos(radians(lc.spot_angle));
    float theta = dot(light_dir, normalize(-lc.dir_));
    
    float d = max(dot(light_dir,normal),0.0);
    vec3 diffuse = light_color * d;
    
    vec3 ambient_color = lc.ambient_color;
    float ambient_power = 0.1;
    vec3 ambient = ambient_color * ambient_power;
    vec3 result;
    
    float light_constant = 1.0f;
    float light_linear = lc.light_linear_;
    float light_quadratic = lc.light_quadratic_;
    float distance = length(light_pos - pos);
    float attenuation = 1.0 / (light_constant + light_linear * distance + light_quadratic * (distance * distance)); 
    
    if (theta > cutOff) {  
        vec3 view_dir = normalize(u_camera_position - pos);
        vec3 reflect_dir = reflect(-light_dir, normal);
        float s = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
        vec3 spec_color = specular_color * s;
        
        diffuse *= attenuation;
        spec_color *= attenuation;
        result = diffuse + spec_color;
    } else {
        result = ambient * diffuse;
    }
    
    return result;
}

void main() {
  vec3 final_color = vec3(0.0, 0.0, 0.0);
  //... Lights are always additive:
  for (int i = 0; i < 8; ++i) {

   // u_light[i].type_ = i;
    if(u_light[i].type_==0){
    final_color += SpotLight(u_light[i]);
    }

    if(u_light[i].type_==1){
    final_color += PointLight(u_light[i]);
    }

  }

  fragColor = vec4(final_color, 1.0);
}