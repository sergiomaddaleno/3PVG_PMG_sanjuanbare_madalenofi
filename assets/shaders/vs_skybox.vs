#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model; // New uniform for the skybox rotation

out vec3 FragPos;

void main() {
   
    vec4 pos = projection * view * model * vec4(aPos, 1.0);
    gl_Position = pos;

    FragPos = aPos; 
}
