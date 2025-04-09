#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 u_camera_pos;

out vec3 v_ray_dir;
out mat4 inverse_view;
out vec3 position;
out vec3 direction;

void main()
{
    mat4 inverse_view = inverse(view);
    position = (inverse_view * vec4(0,0,0,1)).xyz;
    direction = (inverse_view * vec4(0,0,-1,0)).xyz;


    //v_ray_dir = mat3(inverse(view)) * normalize(aPos);

    gl_Position = vec4(aPos, 1.0);
}
