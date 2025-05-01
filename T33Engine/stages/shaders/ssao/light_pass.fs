#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssaoTex;

uniform sampler2D u_shadow_map;
uniform samplerCube u_shadow_map_cube;
uniform int u_ssao_option = 0;

struct LightInfo {
    int type_;
    vec3 pos_;
    vec3 dir_;
    vec3 diff_color_;
    vec3 spec_color_;
    vec3 light_color_;
    vec3 ambient_color_;

    float linear_att;
    float quad_att_;
    float constant_att_;
    float shininess_;
    float strength_;
    float spot_angle_;
    float spot_softness_;
};

uniform LightInfo u_light;
uniform vec3 u_camera_position;

vec3 FragPos;
vec3 normal;
vec3 Diffuse;

// --- Lights
vec3 PointLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float diffuseStrength = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation;

    vec3 viewDir = normalize(u_camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);

    vec3 specular = specStrength * lc.light_color_ * attenuation;

    return (diffuse + specular);
}

vec3 SpotLight(LightInfo lc) {
    vec3 lightDir = normalize(lc.pos_ - FragPos);
    float cutOff = cos(radians(lc.spot_angle_));
    float outerCutOff = cos(radians(lc.spot_angle_ + lc.spot_softness_));
    float theta = dot(lightDir, normalize(-lc.dir_));

    float distance = length(lc.pos_ - FragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float intensity = smoothstep(outerCutOff, cutOff, theta);

    float diffuseStrength = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation * intensity;

    vec3 viewDir = normalize(u_camera_position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);
    vec3 specular = lc.spec_color_ * specStrength * attenuation * intensity;

    vec3 ambient = lc.ambient_color_ * intensity;

    return ambient + diffuse + specular;
}

vec3 DirectionalLight(LightInfo lc) {
   
    vec3 light_dir = normalize(-lc.dir_);

    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * lc.light_color_ * lc.diff_color_ * lc.strength_;

    vec3 view_dir = normalize(u_camera_position - FragPos);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), lc.shininess_);
    vec3 specular = spec * lc.spec_color_;

    vec3 ambient = lc.ambient_color_ * lc.light_color_;

    return ambient + diffuse + specular;
}

void main()
{     
    FragPos = texture(gPosition, uv).rgb;
    normal = texture(gNormal, uv).rgb;
    Diffuse = texture(gAlbedoSpec, uv).rgb;

    float ambient_occlussion = texture(ssaoTex, uv).r;

    vec3 ambient = max(vec3(0.0), 0.3 * Diffuse * (1.0 - ambient_occlussion));
    vec3 final_light = ambient;

    if (u_light.type_ == 0) {
        final_light += DirectionalLight(u_light); 
    }
    
    if (u_light.type_ == 1) {
        final_light += SpotLight(u_light); 
    }
    
    if (u_light.type_ == 2) {
        final_light += PointLight(u_light);  
    }

    vec3 final_color;
    if(u_ssao_option==0){
        final_color = Diffuse * final_light;
    }
    if(u_ssao_option==1){
        final_color =  normal;
    }
    if(u_ssao_option==2){
        final_color =  FragPos;
    }
    if(u_ssao_option==3){
        final_color = vec3(texture(ssaoTex, uv).r);
    }

    FragColor = vec4(final_color, 1.0);
}