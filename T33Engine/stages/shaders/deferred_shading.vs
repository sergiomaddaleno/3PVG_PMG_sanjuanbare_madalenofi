#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 uv;
uniform mat4 u_light_space;
out vec4 FragPosLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool u_useDepthMap;
in vec3 FragPos;

void main()
{
    uv = aTexCoord;
    
    vec3 FragPosli = vec3(model * vec4(aPos, 1.0));
    FragPosLight = u_light_space * vec4(FragPos, 1.0);
    if(!u_useDepthMap){
    gl_Position = vec4(aPos, 1.0);
    }else{
    gl_Position = projection * view * vec4(FragPosli, 1.0);
    }
}