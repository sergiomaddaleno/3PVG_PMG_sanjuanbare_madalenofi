#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
out vec3 normal;
out vec3 FragPos;

uniform mat4 u_light_space;
out vec4 FragPosLightSpace; 

out float visibility;
uniform float u_density = 0.06;
uniform float u_gradient = 2.0;

void main()
{
    

    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    FragPosLightSpace = u_light_space * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);

}