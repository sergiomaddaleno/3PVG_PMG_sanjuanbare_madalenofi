#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 uv;
uniform mat4 u_light_space;
out vec4 FragPosLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    uv = aTexCoord;
    FragPosLight = u_light_space * vec4(aPos, 1.0);
    gl_Position = vec4(aPos, 1.0);
}