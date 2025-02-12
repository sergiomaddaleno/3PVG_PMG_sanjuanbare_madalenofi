#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
out float visibility;

uniform float u_density = 0.06;
uniform float u_gradient = 2.0;

void main()
{
    vec4 world_pos = model * vec4(aPos, 1.0);
    vec4 pos_relative_to_cam = view * world_pos;

    float distance = length(pos_relative_to_cam.xyz);
    visibility = exp(-pow((distance*u_density), u_gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    uv = aTexCoord;
    gl_Position = projection * pos_relative_to_cam;
}