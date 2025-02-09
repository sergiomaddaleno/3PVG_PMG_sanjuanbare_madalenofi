#version 400

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
out vec3 normal;
out vec3 FragPos;

const int kMaxLights = 50;
const int kMaxShadows = 5;

uniform mat4 u_light_space[kMaxShadows];
//uniform u_light_view[kMaxShadows];
//uniform u_light_projection[kMaxShadows];
out vec4 FragPosLightSpace[kMaxShadows];

out float visibility;
uniform float u_density = 0.06;
uniform float u_gradient = 2.0;

void main()
{
    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    for (int i = 0; i < kMaxShadows; i++) {
        FragPosLightSpace[i] = u_light_space[i] * vec4(FragPos, 1.0);
        //gl_Position = u_light_projection[i] * u_light_view[i] * vec4(FragPos, 1.0);
    }

    gl_Position = projection * view * vec4(FragPos, 1.0);

}