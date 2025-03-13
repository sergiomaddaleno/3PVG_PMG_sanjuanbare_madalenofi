#version 330 core

in vec3 FragPos;

uniform vec3 u_light_pos;
uniform float u_far_plane;

void main(){
    float light_distance = length(FragPos.xyz - u_light_pos);

    light_distance = light_distance / u_far_plane;

    gl_FragDepth = light_distance;
}