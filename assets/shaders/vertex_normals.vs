#version 330

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 normal;
out vec3 FragPos;
out vec3 pos;
out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    normal = mat3(transpose(inverse(model))) * aNormal;
    pos = aPos;
    uv = aTexCoord;
    FragPos = aPos;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}