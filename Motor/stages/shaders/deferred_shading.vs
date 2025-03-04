#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 uv;
uniform mat4 u_light_space;
out vec4 FragPosLight;

void main()
{
    uv = aTexCoord;
    gl_Position = vec4(aPos, 1.0);
}