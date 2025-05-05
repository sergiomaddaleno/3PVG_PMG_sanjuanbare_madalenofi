#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float u_outline_scale = 1.05;

void main() {

    vec3 scaledPos = aPos * u_outline_scale;
    gl_Position = projection * view * model * vec4(scaledPos, 1.0);
}
