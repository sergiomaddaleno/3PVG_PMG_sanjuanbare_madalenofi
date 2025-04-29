#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool u_useDepthMap;

void main()
{
    uv = aTexCoord;
    
    if(!u_useDepthMap){
        gl_Position = vec4(aPos, 1.0);
    }else{
        gl_Position = projection * view * model * vec4(aPos, 1.0);;
    }
}