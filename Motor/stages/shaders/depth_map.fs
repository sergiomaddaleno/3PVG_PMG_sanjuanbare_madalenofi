#version 330 core

uniform sampler2D u_depth_map;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 u_light_space;

in vec2 uv;
in vec3 normal;
in vec3 FragPos;
out vec4 FragColor;


void main(){
	float depth_value = texture(u_depth_map, uv).r;
	FragColor = vec4(vec3(depth_value), 1.0);
}