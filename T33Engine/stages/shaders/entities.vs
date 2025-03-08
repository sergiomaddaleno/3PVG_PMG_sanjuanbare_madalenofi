#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 u_camera_position;

out vec2 uv;
out vec3 normal;
out vec3 FragPos;

//Shadows
uniform mat4 u_light_space;
out vec4 FragPosLight;

uniform vec3 u_light_pos_;

//Normal mapping
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

// fog
out float visibility;
uniform float u_density = 0.06;
uniform float u_gradient = 2.0;

// wave effect
uniform float u_time;
uniform float u_wave_amplitude = 0.2;
uniform float u_wave_length = 2.0;
uniform float u_wave_speed = 1.0;
uniform vec2 u_wave_direction = vec2(1.0, 0.5);
uniform bool u_use_wave = false;

vec3 waveEffect(vec3 position) {
    if (!u_use_wave) return position;

    float k = 6.28318530718 / u_wave_length;
    float omega = k * u_wave_speed;
    vec2 dir = normalize(u_wave_direction);

    float wave = u_wave_amplitude * sin(k * dot(position.xz, dir) + omega * u_time);
    position.y += wave;

    return position;
}

void TBN(vec3 lightPos, vec3 viewPos){
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    TangentLightPos = TBN * lightPos;
    TangentViewPos = TBN * viewPos;
    TangentFragPos = TBN * FragPos;
}

void main()
{
    vec3 local_pos = waveEffect(aPos);
    vec4 world_pos = model * vec4(local_pos, 1.0);

    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(local_pos, 1.0));
    FragPosLight = u_light_space * vec4(FragPos, 1.0);

    //Fog
    vec4 pos_relative_to_cam = view * world_pos;
    float distance = length(pos_relative_to_cam.xyz);
    visibility = exp(-pow((distance*u_density), u_gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    //TBN
    TBN(u_light_pos_, u_camera_position);

    gl_Position = projection * view * vec4(FragPos, 1.0);

}