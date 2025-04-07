#version 330 core
out vec4 FragColor;

in vec2 uv;

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

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform sampler2D texNoise;
uniform sampler2D u_shadow_map;
uniform samplerCube u_depth_map;

uniform vec3 samples[64];
uniform mat4 projection;

uniform int u_deferred_option;
uniform LightInfo u_light;
uniform vec3 u_ambient;
uniform float u_far_plane;
uniform mat4 u_light_space;
uniform vec3 u_camera_position;

vec3 FragPos;
vec3 normal;
vec3 Diffuse;

const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0);



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

// --- SSAO Parameters ---
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

float computeSSAO(vec3 fragPos, vec3 normal)
{
    vec3 randomVec = normalize(texture(texNoise, uv * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * radius;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;

        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    return occlusion;
}

void main()
{
    vec3 FragPos = texture(gPosition, uv).rgb;
    vec3 normal = normalize(texture(gNormal, uv).rgb);
    vec3 Diffuse = texture(gAlbedoSpec, uv).rgb;

    vec4 FragPosLight = u_light_space * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(FragPosLight);
    float shadow_pl = ShadowCalculationPoint(FragPos, u_light.pos_);

    float ssao = computeSSAO(FragPos, normal);

    vec3 final_light = u_ambient;

    if (u_light.type_ == 0) {
        final_light += DirectionalLight(u_light) * shadow;
    }
    if (u_light.type_ == 1) {
        final_light += SpotLight(u_light) * shadow;
    }
    if (u_light.type_ == 2) {
        final_light += PointLight(u_light) * shadow_pl;
    }

 
    vec3 final_color = normal ;
    

    FragColor = vec4(final_color, 1.0);
}
