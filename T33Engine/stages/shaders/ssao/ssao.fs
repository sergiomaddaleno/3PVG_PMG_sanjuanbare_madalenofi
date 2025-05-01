#version 330 core
out float FragColor;

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];
uniform mat4 projection;

uniform int u_kernel_size = 64;
uniform float u_radius = 0.5;
uniform float u_bias = 0.025;

// screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

void main()
{
    vec3 fragPos = texture(gPosition, uv).xyz;
    vec3 normal = normalize(texture(gNormal, uv).rgb);
    vec3 randomVec = normalize(texture(texNoise, uv * noiseScale).xyz);

    //TBN - tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < u_kernel_size; ++i)
    {
        vec3 samplePos = TBN * samples[i]; 
        samplePos = fragPos + samplePos * u_radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = texture(gPosition, offset.xy).z; 
        
        float rangeCheck = smoothstep(0.0, 1.0, u_radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth <= samplePos.z + u_bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / u_kernel_size);
    
    FragColor = occlusion;
}