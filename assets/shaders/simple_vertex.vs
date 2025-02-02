#version 330

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec3 normal;
out vec3 pos;
out vec2 uv;

out vec3 FragPos;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    normal = normalize(normalMatrix * a_normal);
    pos = a_position;
    uv = a_uv;
    FragPos = vec3(model * vec4(a_position, 1.0));
    gl_Position = model * view  * projection * vec4(a_position, 1.0);
}
