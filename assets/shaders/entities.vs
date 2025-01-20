#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 mvp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 uv;
out float visibility;
out vec3 normal;
out vec3 FragPos;

const float density = 0.06;
const float gradient = 2.0;

void main()
{
    vec4 world_pos = model * vec4(aPos, 1.0);
    vec4 pos_relative_to_cam = view * world_pos;

    float distance = length(pos_relative_to_cam.xyz);
    visibility = exp(-pow((distance * density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    uv = aTexCoord;
    normal = mat3(transpose(inverse(model))) * aNormal; // Transform normal by model matrix
    FragPos = world_pos.xyz; // Pass world position to FragPos

    gl_Position = projection * pos_relative_to_cam;
}
