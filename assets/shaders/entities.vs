#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightView[3];
uniform mat4 lightProjection[3];

uniform int u_number_lights;
out vec2 uv;
out vec3 normal;
out vec3 FragPos;

const int kMaxLights = 50;
uniform mat4 u_light_space[3];
out vec4 FragPosLightSpace[3]; 
uniform int lightIndex;
out float visibility;
uniform float u_density = 0.06;
uniform float u_gradient = 2.0;
uniform int shadow;

void main()
{
    

    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    for (int i = 0; i < u_number_lights; ++i) {
    FragPosLightSpace[i] = u_light_space[i] * vec4(FragPos, 1.0);
    }

    if(shadow==1){
    gl_Position = lightProjection[lightIndex] * lightView[lightIndex] * vec4(FragPos, 1.0);
    }else{
    gl_Position = projection * view * vec4(FragPos, 1.0);
    }
    

}