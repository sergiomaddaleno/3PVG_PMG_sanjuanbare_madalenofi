#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 cam_matrix;
// We'll use a single scale factor instead of separate matrices
uniform float u_outline_scale; 

void main() {
    // Scale the vertex position outward along its normal (approximating with vertex position)
    vec3 scaledPos = aPos * u_outline_scale;
    
    // Transform to world space then to clip space
    gl_Position = cam_matrix * model * vec4(scaledPos, 1.0);
}
