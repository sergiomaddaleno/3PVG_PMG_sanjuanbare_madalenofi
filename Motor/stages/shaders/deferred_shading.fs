#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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

void main()
{             
   
    vec3 FragPos = texture(gPosition, uv).rgb;
    vec3 Normal = texture(gNormal, uv).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, uv).rgb;
  
    
  
    vec3 lighting  = Diffuse * 0.1; 
    vec3 viewDir  = normalize(u_camera_position - FragPos);
    
       
   float distance = length(u_light.pos_ - FragPos);
        if(distance < u_light.spot_angle_)
        {
          
            vec3 lightDir = normalize(u_light.pos_ - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * u_light.light_color_;
          
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = u_light.light_color_ * spec;
            
            float attenuation = 1.0 / (1.0 + u_light.linear_att * distance + u_light.quad_att_ * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
       
    FragColor = vec4(lighting, 1.0);
}