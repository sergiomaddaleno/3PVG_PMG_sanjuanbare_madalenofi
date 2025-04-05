#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 u_camera_pos;

out vec3 v_ray_dir;

void main()
{
    v_ray_dir = mat3(inverse(view)) * normalize(aPos);

    gl_Position = vec4(aPos, 1.0);
}
