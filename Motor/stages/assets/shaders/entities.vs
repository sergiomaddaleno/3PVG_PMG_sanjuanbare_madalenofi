#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
out vec3 normal;
out vec3 FragPos;

uniform mat4 u_light_space;
out vec4 FragPosLight;

// fog
out float visibility;
uniform float u_density = 0.06;
uniform float u_gradient = 2.0;

// wave effect
out vec3 original_pos;
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

void main()
{
    vec3 local_pos = waveEffect(aPos);

    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(local_pos, 1.0));
    FragPosLight = u_light_space * vec4(FragPos, 1.0);

    vec4 world_pos = model * vec4(local_pos, 1.0);
    vec4 pos_relative_to_cam = view * world_pos;
    float distance = length(pos_relative_to_cam.xyz);
    visibility = exp(-pow((distance*u_density), u_gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    gl_Position = projection * view * vec4(FragPos, 1.0);

}