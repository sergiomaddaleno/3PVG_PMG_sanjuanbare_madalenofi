#version 330

uniform sampler2D u_texture; // Textura de la escena renderizada
    in vec2 uv;
    out vec4 fragColor;

    void main() {
        // Factor de distorsión
        float distortionFactor = 0.03; // Puedes ajustar este valor para cambiar la intensidad de la distorsión

        // Obtener la coordenada de textura distorsionada
        vec2 distortedUV = uv + vec2(sin(uv.y * 10.0), cos(uv.x * 10.0)) * distortionFactor;

        // Obtener el color de la textura distorsionada
        vec4 distortedColor = texture(u_texture, distortedUV);

        // Salida del color distorsionado
        fragColor = distortedColor;
    }
