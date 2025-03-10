#version 330 core

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

in vec3 normal;
in vec3 pos;
in vec2 uv;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 u_camera_position;

//Samplers
uniform sampler2D u_texture;
uniform sampler2D u_normal_map;
uniform sampler2D u_parallax_map;
uniform sampler2D u_shadow_map;
uniform samplerCube u_depth_map;

uniform bool u_use_normal_map;
uniform bool u_use_parallax_map;
uniform int u_parallax_type;
uniform float u_parallax_scale;
vec3 mapped_normal = normal;

//Light
uniform LightInfo u_light;
uniform vec3 u_ambient;
uniform float u_far_plane;
in vec4 FragPosLight;

//TBN
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

//Fog
uniform bool u_fog;
uniform vec3 u_sky_colour;
in float visibility;

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

    vec3 ambient = lc.ambient_color_ * u_ambient * intensity;

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

//Normal mapping
vec3 NormalMappingSpot(LightInfo lc){
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float cutOff = cos(radians(lc.spot_angle_));
    float outerCutOff = cos(radians(lc.spot_angle_ + lc.spot_softness_));
    float theta = dot(lightDir, normalize(lc.dir_));

    float distance = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0 / (lc.constant_att_ + lc.linear_att * distance + lc.quad_att_ * distance * distance);

    float intensity = smoothstep(outerCutOff, cutOff, theta);

    float diffuseStrength = max(dot(lightDir, mapped_normal), 0.0);
    vec3 diffuse = lc.light_color_ * diffuseStrength * attenuation * intensity;

    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, mapped_normal);
    float specStrength = pow(max(dot(viewDir, reflectDir), 0.0), lc.shininess_);
    vec3 specular = lc.spec_color_ * specStrength * attenuation * intensity;

    vec3 ambient = lc.ambient_color_ * u_ambient * intensity;

    return ambient + diffuse + specular;
}

//parallax mapping
vec2 ParallaxMapping(vec2 tex_coords, vec3 view_dir){
    float height = texture(u_parallax_map, tex_coords).r;
    vec2 p = view_dir.xy / view_dir.z * (height * u_parallax_scale);
    return tex_coords - p;
}

vec2 SteepParallaxMapping(vec2 tex_coords, vec3 view_dir){
    
    const float minLayers = 8;
    const float maxLayers = 32;
    float num_layers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));

    float layer_depth = 1.0 / num_layers;
    float current_layer_depth = 0.0;

    vec2 p = view_dir.xy * u_parallax_scale;
    vec2 delta_tex_coords = p / num_layers;

    vec2 current_texcoords = tex_coords;
    float current_parallaxmap = texture(u_parallax_map, current_texcoords).r;

    while(current_layer_depth < current_parallaxmap){
        current_texcoords -= delta_tex_coords;
        current_parallaxmap = texture(u_parallax_map, current_texcoords).r;
        current_layer_depth += layer_depth;
    }

    return current_texcoords;
}

vec2 ParallaxOcclusionMapping(vec2 tex_coords, vec3 view_dir){
    
    const float minLayers = 8;
    const float maxLayers = 32;
    float num_layers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));

    float layer_depth = 1.0 / num_layers;
    float current_layer_depth = 0.0;

    vec2 p = view_dir.xy * u_parallax_scale;
    vec2 delta_tex_coords = p / num_layers;

    vec2 current_texcoords = tex_coords;
    float current_parallaxmap = texture(u_parallax_map, current_texcoords).r;

    while(current_layer_depth < current_parallaxmap){
        current_texcoords -= delta_tex_coords;
        current_parallaxmap = texture(u_parallax_map, current_texcoords).r;
        current_layer_depth += layer_depth;
    }

    vec2 prev_texcoords = current_texcoords + delta_tex_coords;
    float after_depth = current_parallaxmap - current_layer_depth;
    float before_depth = texture(u_parallax_map, prev_texcoords).r - current_layer_depth + layer_depth;

    float weight = after_depth / (after_depth - before_depth);
    vec2 final_texcoords = prev_texcoords * weight + current_texcoords * (1.0 - weight);

    return final_texcoords;
}

// --- Shadows

//percentage-closer filtering
float PCF(float currentDepth, float bias, vec3 projCoords){
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);

    for(int x=-1; x<=1; ++x){
        for(int y=-1; y<1; ++y){
            
            float pcfDepth = texture(u_shadow_map, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;  
    projCoords = projCoords * 0.5 + 0.5; 

    float closestDepth = texture(u_shadow_map, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = 0.0005;

    //return (currentDepth - bias) > closestDepth ? 0.0 : 1.0;
    return 1.0 - PCF(currentDepth, bias, projCoords);
}

float ShadowCalculationPoint(vec3 fragPos, vec3 lightPos) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight); 
    
    float closestDepth = texture(u_depth_map, fragToLight).r;
    closestDepth *= u_far_plane; 

    float bias = 0.05; 
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return 1.0 - shadow; 
}

void main() {

    vec3 final_light = u_ambient;
    float shadow = ShadowCalculation(FragPosLight);
    float shadow_pl = ShadowCalculationPoint(FragPosLight.xyz, u_light.pos_);

    //Parallax mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = uv;

    if(u_use_parallax_map){
        if(u_parallax_type == 0) texCoords = ParallaxMapping(texCoords, viewDir);
        if(u_parallax_type == 1) texCoords = SteepParallaxMapping(texCoords, viewDir);
        if(u_parallax_type == 2) texCoords = ParallaxOcclusionMapping(texCoords, viewDir);

        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;
    }

    //Normal mapping
    mapped_normal = texture(u_normal_map, texCoords).rgb;
    mapped_normal = normalize(mapped_normal * 2.0 - 1.0);

    //Lights + shadows
    if (u_light.type_ == 0) {
        final_light += DirectionalLight(u_light) * shadow ; 
    }
    
    if (u_light.type_ == 1) {

        if (u_use_normal_map) {
            mat3 TBN = mat3(TangentLightPos, TangentViewPos, TangentFragPos); 
            mapped_normal = normalize(TBN * mapped_normal);

            final_light += NormalMappingSpot(u_light) * shadow; 
        } else {
            final_light += SpotLight(u_light) * shadow; 
        }
    }
    
    if (u_light.type_ == 2) {
        final_light += PointLight(u_light) * shadow;  
    }

    vec3 texture_color = texture(u_texture, texCoords).rgb;

    vec3 final_color = texture_color * final_light;
    FragColor = vec4(final_color, 1.0);

    if(u_fog) FragColor = mix(vec4(u_sky_colour, 1.0), FragColor, visibility);
}