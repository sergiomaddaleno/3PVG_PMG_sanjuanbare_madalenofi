#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;



in vec2 uv;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D u_shadow_map;

void main()
{    
    gPosition = FragPos;
    gNormal = normalize(Normal);
    
    vec4 albedo = texture(texture_diffuse, uv);
    gAlbedoSpec.rgb = albedo.rgb;
    vec4 shadowSample = texture(u_shadow_map, uv);
    gAlbedoSpec.a = shadowSample.r;  
}