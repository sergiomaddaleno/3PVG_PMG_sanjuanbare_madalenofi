#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;

void main() {

    vec4 pos = projection * vec4(mat3(view) * aPos, 1.0);
    gl_Position = pos;
        
    FragPos = aPos;
}
