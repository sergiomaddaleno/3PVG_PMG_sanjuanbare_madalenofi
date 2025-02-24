#version 330

out vec4 FragColor;
in vec3 normal;
in vec3 FragPos;
in vec3 pos;
in vec2 uv;

void main(){
    vec3 norm = normalize(normal);
    FragColor = vec4(norm, 1.0);
}