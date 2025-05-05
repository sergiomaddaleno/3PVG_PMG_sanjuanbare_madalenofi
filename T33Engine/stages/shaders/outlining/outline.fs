#version 330 core

out vec4 FragColor;

uniform vec4 u_outline_color = vec4(1.0, 1.0, 1.0, 1.0);

void main(){

	FragColor = u_outline_color;
}