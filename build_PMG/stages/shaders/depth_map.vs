#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 u_light_space;

out vec2 uv;
out vec3 normal;
out vec3 FragPos;

void main(){

	uv = aTexCoords;
    normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0));

	gl_Position = u_light_space * model * vec4(aPos, 1.0);
}