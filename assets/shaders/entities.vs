#version 330 core

layout(location = 0) in vec3 aPos;        // Posición del vértice
layout(location = 1) in vec3 aNormal;     // Normal del vértice
layout(location = 2) in vec2 aTexCoord;   // Coordenadas de textura

uniform mat4 mvp;                          // Matriz de Proyección * Vista * Modelo
uniform mat4 model;                        // Matriz de modelo
uniform mat4 view;                         // Matriz de vista
uniform mat4 projection;                   // Matriz de proyección
uniform mat4 lightSpaceMatrix;             // Matriz de proyección de la luz (para sombras)

out vec2 uv;                               // Coordenadas de textura
out float visibility;                      // Visibilidad (para niebla)
out vec3 normal;                           // Normal del vértice transformada
out vec3 FragPos;                          // Posición del fragmento en el espacio mundial
out vec4 FragPosLightSpace;                // Posición en el espacio de la luz

const float density = 0.06;                // Densidad de la niebla
const float gradient = 2.0;                // Gradiente de la niebla

void main()
{

    vec4 world_pos = model * vec4(aPos, 1.0);

  
    vec4 pos_relative_to_cam = view * world_pos;

   
    float distance = length(pos_relative_to_cam.xyz);
    visibility = exp(-pow((distance * density), gradient));
    visibility = clamp(visibility, 0.0, 1.0);

    
    uv = aTexCoord;

    
    normal = mat3(transpose(inverse(model))) * aNormal;

   
    FragPos = world_pos.xyz;

    
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

   
    gl_Position = projection * pos_relative_to_cam;
}

