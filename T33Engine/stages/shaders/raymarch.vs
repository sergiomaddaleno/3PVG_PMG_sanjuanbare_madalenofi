#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 world_pos;
out vec3 ray_origin;
out vec3 ray_direction;

uniform vec3 u_camera_pos;
uniform vec3 u_cloud_pos;

void main()
{
    vec4 loc_world_pos = model * vec4(aPos, 1.0);
    world_pos = loc_world_pos.xyz;
    ray_origin = u_camera_pos - u_cloud_pos;
    ray_direction = world_pos - u_camera_pos;

    gl_Position = projection * view * vec4(world_pos, 1.0);
}
